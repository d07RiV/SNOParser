#include "parser.h"
#include "snomap.h"
#include <algorithm>

uint32 HashName(std::string const& str) {
  uint32 hash = 0;
  for (uint8 c : str) {
    hash = hash * 33 + c;
  }
  return hash;
}
uint32 HashNameLower(std::string const& str) {
  uint32 hash = 0;
  for (uint8 c : str) {
    hash = hash * 33 + std::tolower(c);
  }
  return hash;
}

__declspec(thread) SnoParser* SnoParser::context = nullptr;

SnoSysLoader::SnoSysLoader(std::string dir)
  : dir_(dir)
{
  if (dir.empty()) {
    dir_ = path::root();
  }
}

std::vector<std::string> SnoSysLoader::listdir(SnoInfo const& type) {
  WIN32_FIND_DATA data;
  HANDLE hFind = FindFirstFile((dir_ / type.type / "*" + type.ext).c_str(), &data);
  std::vector<std::string> list;
  if (hFind == INVALID_HANDLE_VALUE) return list;
  do {
    if (!strcmp(data.cFileName, ".") || !strcmp(data.cFileName, "..")) continue;
    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
    list.push_back(path::title(data.cFileName));
  } while (FindNextFile(hFind, &data));
  FindClose(hFind);
  return list;
}
File SnoSysLoader::loadfile(SnoInfo const& type, char const* name) {
  return File(dir_ / type.type / name + type.ext, "rb");
}

//SnoSysLoader SnoSysLoader::default("");
SnoLoader* SnoLoader::default = nullptr;// &SnoSysLoader::default;

#pragma warning(disable: 4005)
#ifdef _WIN64
#include "CascLib64/CascLib.h"
#else
#include "CascLib/CascLib.h"
#endif

static bool stringMatch(char const* pattern, char const* str) {
  for (int i = 0; pattern[i] && pattern[i] != '*'; ++i) {
    if (str[i] != pattern[i]) return false;
  }
  for (int i = strlen(pattern) - 1, j = strlen(str) - 1; i >= 0 && pattern[i] != '*'; --i, --j) {
    if (j < 0 || str[j] != pattern[i]) return false;
  }
  return true;
}

std::vector<std::string> SnoCascLoader::listdir(SnoInfo const& type) {
  CASC_FIND_DATA data;
  std::vector<std::string> list;
  std::string pattern = fmtstring("Base\\%s\\*%s", type.type, type.ext);
  HANDLE hFind = CascFindFirstFile(handle_, pattern.c_str(), &data, nullptr);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      if (stringMatch(pattern.c_str(), data.szFileName)) {
        list.push_back(path::title(data.szPlainName));
      }
    } while (CascFindNextFile(hFind, &data));
    CascFindClose(hFind);
  }
  if (!lang_.empty()) {
    std::string pattern = fmtstring("%s\\%s\\*%s", lang_.c_str(), type.type, type.ext);
    HANDLE hFind = CascFindFirstFile(handle_, pattern.c_str(), &data, nullptr);
    if (hFind != INVALID_HANDLE_VALUE) {
      do {
        if (stringMatch(pattern.c_str(), data.szFileName)) {
          list.push_back(path::title(data.szPlainName));
        }
      } while (CascFindNextFile(hFind, &data));
      CascFindClose(hFind);
    }
  }
  std::sort(list.begin(), list.end());
  list.resize(std::unique(list.begin(), list.end()) - list.begin());
  return list;
}

class CascFileBuffer : public FileBuffer {
  HANDLE file_;
  uint64 pos_;
  uint64 size_;
public:
  CascFileBuffer(HANDLE file)
    : file_(file)
    , pos_(0)
  {
    size_ = size();
  }
  ~CascFileBuffer() {
    CascCloseFile(file_);
  }

  uint64 tell() const {
    return pos_;
  }
  void seek(int64 pos, int mode) {
    pos_ = CascSetFilePointer(file_, pos, nullptr, mode);
  }
  uint64 size() {
    return CascGetFileSize(file_, nullptr);
  }

  size_t read(void* ptr, size_t size) {
    DWORD nread;
    if (!CascReadFile(file_, ptr, size, &nread)) return 0;
    pos_ += nread;
    return nread;
  }
  size_t write(void const* ptr, size_t size) {
    return 0;
  }
};

File SnoCascLoader::cascFile(HANDLE hFile) {
  return File(new CascFileBuffer(hFile));
}

File SnoCascLoader::loadfile(SnoInfo const& type, char const* name) {
  HANDLE pFile;
  if (!lang_.empty() && CascOpenFile(handle_, fmtstring("%s\\%s\\%s%s", lang_.c_str(), type.type, name, type.ext).c_str(), 0, 0, &pFile)) {
    return File(new CascFileBuffer(pFile));
  }
  if (CascOpenFile(handle_, fmtstring("Base\\%s\\%s%s", type.type, name, type.ext).c_str(), 0, 0, &pFile)) {
    return File(new CascFileBuffer(pFile));
  }
  return File();
}
SnoCascLoader::SnoCascLoader(std::string dir, std::string lang)
  : lang_(lang)
  , hash_(HashNameLower(dir))
  , build_(0)
{
  if (!CascOpenStorage(dir.c_str(), CASC_LOCALE_ALL, &handle_)) {
    throw Exception("failed to open CASC storage");
  }

  if (lang == "choose") {
    std::vector<std::string> locales = {"enUS", "deDE", "esES", "esMX", "frFR", "itIT", "koKR", "plPL", "ptBR", "ptPT", "ruRU", "zhTW", "zhCN"};
    std::vector<std::string> lchoose;
    for (std::string const& loc : locales) {
      HANDLE hFile;
      if (CascOpenFile(handle_, (loc + "/StringList/Items.stl").c_str(), 0, 0, &hFile)) {
        CascCloseFile(hFile);
        lchoose.push_back(loc);
      }
    }
    if (lchoose.size() == 0) {
      lang_ = "";
      Logger::log("Warning: no language files");
    } else if (lchoose.size() == 1) {
      lang_ = lchoose[0];
      Logger::log("Using locale: %s", lang.c_str());
    } else{
      int opt = Logger::menu("Choose locale", lchoose);
      lang_ = lchoose[opt];
    }
  }

  HANDLE hFile;
  if (CascOpenFile(handle_, "Base/CoreTOC.dat", 0, 0, &hFile)) {
    DWORD size = CascGetFileSize(hFile, NULL);
    std::vector<uint8> toc(size);
    CascReadFile(hFile, toc.data(), toc.size(), &size);
    CascCloseFile(hFile);

    SnoManager::loadTOC(toc.data());
  }

  while (dir.size() && GetFileAttributes((dir / ".build.info").c_str()) == INVALID_FILE_ATTRIBUTES) {
    dir = path::path(dir);
  }
  File info(dir / ".build.info");
  if (!info) return;
  std::string line;
  if (!info.getline(line)) return;
  size_t pos = line.find("Version!");
  if (pos == std::string::npos) return;
  size_t count = 0;
  while (pos) {
    count += (line[--pos] == '|');
  }
  if (!info.getline(line)) return;
  size_t left = 0;
  while (count--) {
    left = line.find('|', left);
    if (left == std::string::npos) return;
    ++left;
  }
  size_t right = line.find('|', left);
  version_ = line.substr(left, right == std::string::npos ? right : right - left);
  left = line.find_last_of('.', right);
  if (left == std::string::npos) return;
  build_ = std::stoi(line.substr(left + 1, right == std::string::npos ? right : right - left - 1));
}
SnoCascLoader::~SnoCascLoader() {
  CascCloseStorage(handle_);
}
