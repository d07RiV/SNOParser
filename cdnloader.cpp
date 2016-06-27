#include "parser.h"
#include "ngdp.h"
#include "snomap.h"
#include <memory>
#include <set>

struct FileEntry {
  NGDP::Hash_container hash;
  uint32 index;
};
struct TocEntry {
  uint32 asset;
  uint32 index;
  uint32 name;
};
struct ParsedTocEntry {
  uint32 asset = 0;
  char const* name = nullptr;
};

struct TocHeader {
  static const uint32 MAX_ASSETS = 70;
  uint32 entryCounts[MAX_ASSETS];
  uint32 entryOffsets[MAX_ASSETS];
  uint32 unknown[MAX_ASSETS];
  uint32 align;
};

struct SnoCdnLoader::CdnImpl {
  static NGDP::NGDP& get_ngdp() {
    static NGDP::NGDP ngdp;
    return ngdp;
  }
  static NGDP::ArchiveIndex& get_archives() {
    static NGDP::ArchiveIndex archives(get_ngdp());
    return archives;
  }

  std::map<std::string, std::string> buildConfig;
  std::map<istring, NGDP::Hash_container> namedFiles;
  std::unique_ptr<NGDP::Encoding> encoding;
  uint32 build;
  std::string version;

  std::map<istring, NGDP::Hash_container> fileIndex[TocHeader::MAX_ASSETS];

  File load(const NGDP::Hash hash);

  CdnImpl(std::string const& build, std::string lang);
};

SnoCdnLoader::CdnImpl::CdnImpl(std::string const& build, std::string lang) {
  auto& ngdp = get_ngdp();

  File file = ngdp.load(build);
  if (!file) throw Exception("failed to load build %s", build.c_str());
  buildConfig = NGDP::ParseConfig(file);

  auto encodingHashes = split(buildConfig["encoding"]);
  if (encodingHashes.size() != 2) throw Exception("failed to parse build config");
  File rawFile = ngdp.load(encodingHashes[1], "data", false, "Fetching encoding file");
  if (!rawFile) throw Exception("failed to load encoding file");
  encoding.reset(new NGDP::Encoding(NGDP::DecodeBLTE(rawFile, stoi(split(buildConfig["encoding-size"])[0]))));
  rawFile.release();

  auto& archives = get_archives();

  NGDP::Hash hash;
  NGDP::from_string(hash, buildConfig["root"]);
  File root = load(hash);
  if (!root || root.read32() != 0x8007D0C4) {
    throw Exception("invalid root file");
  }


  uint32 count = root.read32(0);

  if (lang == "choose") {
    std::vector<std::string> locales = {"enUS", "deDE", "esES", "esMX", "frFR", "itIT", "koKR", "plPL", "ptBR", "ptPT", "ruRU", "zhTW", "zhCN"};
    std::set<istring> folders;
    for (uint32 i = 0; i < count; ++i) {
      root.seek(16, SEEK_CUR);
      std::string name;
      while (char chr = root.getc()) {
        name.push_back(chr);
      }
      folders.insert(name);
    }

    std::vector<std::string> lchoose;
    for (std::string const& loc : locales) {
      if (folders.count(loc)) {
        lchoose.push_back(loc);
      }
    }
    if (lchoose.size() == 0) {
      lang = "";
      Logger::log("Warning: no language files");
    } else if (lchoose.size() == 1) {
      lang = lchoose[0];
      Logger::log("Using locale: %s", lang.c_str());
    } else{
      int opt = Logger::menu("Choose locale", lchoose);
      lang = lchoose[opt];
    }

    root.seek(8);
  }

  std::vector<FileEntry> files;
  while (count--) {
    root.read(hash, sizeof hash);
    std::string name;
    while (char chr = root.getc()) {
      name.push_back(chr);
    }

    if (name == "Base" || name == lang) {
      File sub = load(hash);
      if (!sub || sub.read32() != 0xEAF1FE87) throw Exception("failed to parse directory structure");
      uint32 count1 = sub.read32();
      while (count1--) {
        files.emplace_back();
        sub.read(&files.back(), sizeof(FileEntry));
      }
      uint32 count2 = sub.read32();
      sub.seek(count2 * 24, SEEK_CUR);
      uint32 count3 = sub.read32();
      while (count3--) {
        NGDP::Hash_container subhash;
        sub.read(&subhash, sizeof subhash);
        std::string subname;
        while (char chr = sub.getc()) {
          subname.push_back(chr);
        }
        namedFiles[subname] = subhash;
      }
    }
  }

  File tocFile = load(namedFiles["CoreTOC.dat"]._);
  if (!tocFile) throw Exception("failed to load CoreTOC.dat");
  std::vector<uint8> toc(tocFile.size());
  tocFile.read(toc.data(), toc.size());
  TocHeader* header = (TocHeader*) toc.data();
  uint8* tocBase = toc.data() + sizeof(TocHeader);

  uint32 maxIndex = 0;
  for (uint32 i = 0; i < TocHeader::MAX_ASSETS; ++i) {
    TocEntry* entries = (TocEntry*) (tocBase + header->entryOffsets[i]);
    for (uint32 j = 0; j < header->entryCounts[i]; ++j) {
      if (entries[j].index >= maxIndex) {
        maxIndex = entries[j].index + 1;
      }
    }
  }
  std::vector<ParsedTocEntry> tocEntries(maxIndex);
  for (uint32 i = 0; i < TocHeader::MAX_ASSETS; ++i) {
    TocEntry* entries = (TocEntry*) (tocBase + header->entryOffsets[i]);
    char const* names = (char const*) (entries + header->entryCounts[i]);
    for (uint32 j = 0; j < header->entryCounts[i]; ++j) {
      ParsedTocEntry& entry = tocEntries[entries[j].index];
      entry.asset = entries[j].asset;
      entry.name = names + entries[j].name;
    }
  }

  for (FileEntry& file : files) {
    if (file.index < maxIndex && tocEntries[file.index].name) {
      ParsedTocEntry& entry = tocEntries[file.index];
      fileIndex[entry.asset][entry.name] = file.hash;
    }
  }

  std::string const& name = buildConfig["build-name"];
  for (auto const& p : split_multiple(name, "_- ")) {
    if (!p.empty() && std::isdigit((unsigned char) p[0])) {
      uint32 ver = 0, i;
      for (i = 0; i < p.size() && std::isdigit((unsigned char) p[i]); ++i) {
        ver = ver * 10 + (p[i] - '0');
      }
      if (ver >= 10000) {
        this->build = ver;
      } else {
        if (!version.empty()) version.push_back('.');
        version.append(p.substr(0, i));
      }
    }
  }
  if (!version.empty()) version.push_back('.');
  version.append(fmtstring("%d", this->build));

  SnoManager::loadTOC(toc.data());
}

File SnoCdnLoader::CdnImpl::load(const NGDP::Hash hash) {
  auto* entry = encoding->getEncoding(hash);
  File raw = get_archives().load(entry->keys[0]);
  if (!raw) return raw;
  return NGDP::DecodeBLTE(raw, entry->usize);
}

SnoCdnLoader::SnoCdnLoader(std::string const& build, std::string lang)
  : handle_(new CdnImpl(build, lang))
  , hash_(HashNameLower(build))
{
}
SnoCdnLoader::~SnoCdnLoader() {
  delete handle_;
}

std::vector<std::string> SnoCdnLoader::builds() {
  auto& ngdp = CdnImpl::get_ngdp();
  File cdn = ngdp.load(ngdp.version().cdn);
  if (!cdn) return std::vector<std::string>();
  auto config = NGDP::ParseConfig(cdn);
  return split(config["builds"]);
}
std::map<std::string, std::string> SnoCdnLoader::buildinfo(std::string const& build) {
  auto& ngdp = CdnImpl::get_ngdp();
  File file = ngdp.load(build);
  if (!file) return std::map<std::string, std::string>();
  return NGDP::ParseConfig(file);
}

File SnoCdnLoader::loadfile(SnoInfo const& type, char const* name) {
  auto it = handle_->fileIndex[type.index].find(name);
  if (it == handle_->fileIndex[type.index].end()) {
    return File();
  } else {
    return handle_->load(it->second._);
  }
}

std::vector<std::string> SnoCdnLoader::listdir(SnoInfo const& type) {
  std::vector<std::string> keys;
  for (auto const& kv : handle_->fileIndex[type.index]) {
    keys.push_back(kv.first);
  }
  return keys;
}

std::map<std::string, std::string> const& SnoCdnLoader::buildinfo() {
  return handle_->buildConfig;
}

File SnoCdnLoader::load(std::string const& hash) {
  auto it = handle_->namedFiles.find(hash);
  if (it == handle_->namedFiles.end()) {
    NGDP::Hash bhash;
    NGDP::from_string(bhash, hash);
    return handle_->load(bhash);
  } else {
    return handle_->load(it->second._);
  }
}

std::map<istring, std::string> SnoCdnLoader::install() {
  std::map<istring, std::string> result;

  NGDP::Hash hash;
  NGDP::from_string(hash, handle_->buildConfig["install"]);
  File file = handle_->load(hash);
  if (!file || file.read16(true) != 'IN') return result;
  file.seek(2, SEEK_CUR);
  uint32 numTags = file.read16(true);
  uint32 numEntries = file.read32(true);
  while (numTags--) {
    while (file.getc());
    file.seek(2 + (numEntries + 7) / 8, SEEK_CUR);
  }
  while (numEntries--) {
    istring name;
    while (char chr = file.getc()) {
      name.push_back(chr);
    }
    file.read(hash, sizeof hash);
    result[name] = NGDP::to_string(hash);
    file.seek(4, SEEK_CUR);
  }
  return result;
}

uint32 SnoCdnLoader::build() const {
  return handle_->build;
}
std::string SnoCdnLoader::version() const {
  return handle_->version;
}
