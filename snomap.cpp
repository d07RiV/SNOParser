#include "snomap.h"
#include "file.h"
#include "types/GameBalance.h"
#include <Windows.h>

void SnoMap::parse(File& file, std::string const& name) {
  if (!file) return;
  if (file.read32() == 0xDEADBEEF) {
    file.seek(16);
    map_[file.read32()] = name;
  }
}
void SnoMap::save(std::string const& type) {
  File file(path::work() / fmtstring("sno_%s", SnoLoader::default->version().c_str()) / type + ".txt", "wt");
  for (const auto& kv : map_) {
    file.printf("%d %s\n", kv.first, kv.second.c_str());
  }
}
bool SnoMap::load(std::string const& type) {
  File file(path::work() / fmtstring("sno_%s", SnoLoader::default->version().c_str()) / type + ".txt", "rt");
  if (!file) return false;
  int id;
  char fname[512];
  for (auto& line : file) {
    size_t space = line.find(' ');
    if (space != std::string::npos) {
      map_[atoi(line.c_str())] = line.substr(space + 1);
    }
  }
  return true;
}

template<class T>
static void insert(std::map<uint32, std::string>& dst, T const& src) {
  for (auto& entry : src) {
    std::string name((char*)&entry);
    dst[HashNameLower(name)] = name;
  }
}

const SnoMap& SnoManager::gameBalance() {
  auto it = instance_.map_.find(0xDEADBEEF);
  if (it == instance_.map_.end()) {
    SnoMap& map = instance_.map_[0xDEADBEEF];
    if (map.load("GameBalanceId")) return map;
    for (auto& gmb : SnoLoader::All<GameBalance>()) {
      insert(map.map_, gmb->x018_ItemTypes);
      insert(map.map_, gmb->x028_Items);
      insert(map.map_, gmb->x078_AffixTable);
      insert(map.map_, gmb->x088_Heros);
      insert(map.map_, gmb->x098_MovementStyles);
      insert(map.map_, gmb->x0A8_Labels);
      insert(map.map_, gmb->x0C8_RareItemNamesTable);
      insert(map.map_, gmb->x0D8_MonsterAffixesTable);
      insert(map.map_, gmb->x0E8_RareMonsterNamesTable);
      insert(map.map_, gmb->x0F8_SocketedEffectsTable);
      insert(map.map_, gmb->x108_ItemDropTable);
      insert(map.map_, gmb->x128_QualityClassTable);
      insert(map.map_, gmb->x158_Hirelings);
      insert(map.map_, gmb->x168_SetItemBonusTable);
      insert(map.map_, gmb->x178_EliteModifiers);
      insert(map.map_, gmb->x198_PowerFormulaTable);
      insert(map.map_, gmb->x1A8_RecipesTable);
      insert(map.map_, gmb->x1B8_ScriptedAchievementEventsTable);
      insert(map.map_, gmb->x1C8_LootRunQuestTierTable);
      insert(map.map_, gmb->x1D8_ParagonBonusesTable);
      insert(map.map_, gmb->x1E8_LegacyItemConversionTable);
      insert(map.map_, gmb->x218_TransmuteRecipesTable);
    }
    map.save("GameBalanceId");
    return map;
  } else {
    return it->second;
  }
}

void SnoManager::clear() {
  std::string root = path::work() / fmtstring("sno_%s", SnoLoader::default->version().c_str());
  WIN32_FIND_DATA fdata;
  HANDLE hFind = FindFirstFile((root / "*").c_str(), &fdata);
  if (hFind == INVALID_HANDLE_VALUE) return;
  std::vector<std::string> list;
  do {
    if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      list.push_back(fdata.cFileName);
    }
  } while (FindNextFile(hFind, &fdata));
  FindClose(hFind);
  for (std::string const& name : list) {
    DeleteFile((root / name).c_str());
  }
}

SnoManager SnoManager::instance_;

struct TocHeader {
  static const uint32 MAX_ASSETS = 70;
  uint32 entryCounts[MAX_ASSETS];
  uint32 entryOffsets[MAX_ASSETS];
  uint32 unknown[MAX_ASSETS];
  uint32 align;
};
struct TocEntry {
  uint32 asset;
  uint32 index;
  uint32 name;
};

void SnoManager::loadTOC(uint8 const* toc) {
  TocHeader const* header = (TocHeader const*) toc;
  toc += sizeof(TocHeader);
  for (size_t i = 0; i < TocHeader::MAX_ASSETS; ++i) {
    if (!header->entryCounts[i]) continue;

    TocEntry const* entries = (TocEntry const*)(toc + header->entryOffsets[i]);
    char const* names = (char const*)(entries + header->entryCounts[i]);

    for (size_t j = 0; j < header->entryCounts[i]; ++j) {
      instance_.map_[entries[j].asset].map_[entries[j].index] = names + entries[j].name;
    }
  }
}
