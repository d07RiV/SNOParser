#include "parser.h"
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

std::vector<std::string> SnoSysLoader::listdir(char const* type, char const* ext) {
  WIN32_FIND_DATA data;
  HANDLE hFind = FindFirstFile((dir_ / type / "*" + ext).c_str(), &data);
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
File SnoSysLoader::loadfile(char const* type, char const* name, char const* ext) {
  return File(dir_ / type / name + ext, "rb");
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

std::vector<std::string> SnoCascLoader::listdir(char const* type, char const* ext) {
  CASC_FIND_DATA data;
  std::vector<std::string> list;
  std::string pattern = fmtstring("Base\\%s\\*%s", type, ext);
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
    std::string pattern = fmtstring("%s\\%s\\*%s", lang_.c_str(), type, ext);
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

File SnoCascLoader::loadfile(char const* type, char const* name, char const* ext) {
  HANDLE pFile;
  if (!lang_.empty() && CascOpenFile(handle_, fmtstring("%s\\%s\\%s%s", lang_.c_str(), type, name, ext).c_str(), 0, 0, &pFile)) {
    return File(new CascFileBuffer(pFile));
  }
  if (CascOpenFile(handle_, fmtstring("Base\\%s\\%s%s", type, name, ext).c_str(), 0, 0, &pFile)) {
    return File(new CascFileBuffer(pFile));
  }
  return File();
}
SnoCascLoader::SnoCascLoader(std::string dir, std::string lang)
  : lang_(lang)
  , hash_(HashNameLower(dir))
{
  if (!CascOpenStorage(dir.c_str(), CASC_LOCALE_ALL, &handle_)) {
    throw Exception("failed to open CASC storage");
  }
}
SnoCascLoader::~SnoCascLoader() {
  CascCloseStorage(handle_);
}
