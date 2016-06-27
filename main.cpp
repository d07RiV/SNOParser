#include <stdio.h>
#include <iostream>
#include <clocale>
#include <conio.h>
#include <set>
#include "snomap.h"
#include "strings.h"
#include "stringmgr.h"
#include "parser.h"
#include "json.h"
#include "path.h"
#include "image.h"
#include "textures.h"
#include "types/Textures.h"
#include "types/Appearance.h"
#include "types/Actor.h"
#include "types/Monster.h"
#include "types/AnimSet.h"
#include "types/StringList.h"
#include "types/SkillKit.h"
#include "regexp.h"
#include "translations.h"
#include "description.h"
#include "miner.h"
#include "powertag.h"
#include "model.h"
#include "itemlib.h"
#include "webgl.h"
#include "frameui/searchlist.h"
#include <map>
#include <vector>
#include <algorithm>
#include <stdarg.h>

//#define MODELVIEWER
#define PTR

namespace path {
#ifndef MODELVIEWER
  std::vector<std::string> roots {
    "C:\\Work\\junk",
    "C:\\Webroot\\game",
    "C:\\tmp",
  };
  std::vector<std::string> cascs {
#ifdef PTR
    //"F:\\Games\\D3Test\\Data",
    //"E:\\Games\\D3Test\\Data",
    "F:\\Games\\Diablo III\\Data",
    //"E:\\Games\\Diablo III Public Test\\Data",
    //"C:\\Webroot\\game\\D3T2\\Data",
    //"E:\\Games\\Diablo III\\Data",
#else
    "E:\\D3Live\\Data",
    "G:\\D3Live\\Data",
#endif
  };
#else
  std::vector<std::string> roots;
  std::vector<std::string> cascs;
#endif
}

Model* ShowModel(Model* model) {
  for (uint32 i = 0; i < model->numSubObjects(); ++i) {
    //if (model->subObjectType(i) == 103) {
      model->appearance(i, 0);
    //}
  }
  return model;
}

std::string getItemAppearance(char const* id) {
  auto* item = ItemLibrary::get(id);
  if (!item) {
    SnoFile<Actor> actor(id);
    if (!actor) return std::string();
    return actor->x014_AppearanceSno.name().c_str();
  }
  SnoFile<Actor> actor(item->x108_ActorSno.name());
  if (!actor) return std::string();
  return actor->x014_AppearanceSno.name().c_str();
}

Model* Equip(Model& model, char const* hp, char const* id) {
  std::string app = getItemAppearance(id);
  if (!app.empty()) {
    return ShowModel(model.attach(hp, app.c_str()));
  } else {
    SnoFile<Actor> actor(id);
    if (actor) return ShowModel(model.attach(hp, actor->x014_AppearanceSno.name().c_str()));
    else return ShowModel(model.attach(hp, id));
  }
  return nullptr;
}

std::map<std::string, uint32> get_item_icons(bool leg = false);
void dump_data(std::string const& suffix);
void make_menu();
void make_diffs();
void dump_affixes();
void archon();

template<class T>
void SnoMerge(std::map<uint32, std::string>& dst) {
  auto& src = SnoManager::get<T>().get();
  for (auto& kv : src) {
    dst[kv.first] = kv.second;
  }
}
std::map<uint32, std::string> SuperSno() {
  std::map<uint32, std::string> res;
  SnoMerge<Accolade>(res);
  SnoMerge<Act>(res);
  SnoMerge<Actor>(res);
  SnoMerge<Adventure>(res);
  SnoMerge<AmbientSound>(res);
  SnoMerge<Anim>(res);
  SnoMerge<Anim2D>(res);
  SnoMerge<AnimSet>(res);
  SnoMerge<Appearance>(res);
  SnoMerge<BossEncounter>(res);
  SnoMerge<Cloth>(res);
  SnoMerge<Condition>(res);
  SnoMerge<Conversation>(res);
  SnoMerge<EffectGroup>(res);
  SnoMerge<Encounter>(res);
  SnoMerge<Explosion>(res);
  SnoMerge<Font>(res);
  SnoMerge<GameBalance>(res);
  SnoMerge<Globals>(res);
  SnoMerge<LevelArea>(res);
  SnoMerge<Light>(res);
  SnoMerge<Lore>(res);
  SnoMerge<MarkerSet>(res);
  SnoMerge<Material>(res);
  SnoMerge<Monster>(res);
  SnoMerge<Music>(res);
  SnoMerge<Observer>(res);
  SnoMerge<Particle>(res);
  SnoMerge<Physics>(res);
  SnoMerge<PhysMesh>(res);
  SnoMerge<Power>(res);
  SnoMerge<Quest>(res);
  SnoMerge<QuestRange>(res);
  SnoMerge<Recipe>(res);
  SnoMerge<Reverb>(res);
  SnoMerge<Rope>(res);
  SnoMerge<Scene>(res);
  SnoMerge<SceneGroup>(res);
  SnoMerge<ShaderMap>(res);
  SnoMerge<Shaders>(res);
  SnoMerge<Shakes>(res);
  SnoMerge<SkillKit>(res);
  SnoMerge<Sound>(res);
  SnoMerge<SoundBank>(res);
  SnoMerge<Surface>(res);
  SnoMerge<Textures>(res);
  SnoMerge<Trail>(res);
  SnoMerge<Tutorial>(res);
  SnoMerge<UI>(res);
  SnoMerge<Weather>(res);
  SnoMerge<Worlds>(res);
  SnoMerge<StringList>(res);
  return res;
}

void write_tex(std::string const& name, ImageFormat::Type format = ImageFormat::PNG) {
  SnoFile<Textures> tex(name);
  tex->load(0).write("Tex/" + name + ".png", format);
}
void item_flavor(SnoLoader* loader = SnoLoader::default);
void translate_extra();

std::string ActorApp(uint32 aid) {
  SnoFile<Actor> actor(Actor::name(aid));
  if (!actor) return std::string();
  return actor->x014_AppearanceSno.name();
}

std::string ItemModel(std::string const& name) {
  auto* item = ItemLibrary::get(name);
  SnoFile<Actor> actor(item->x108_ActorSno.name());
  if (!actor) return "";
  return actor->x014_AppearanceSno.name();
}
void dump_animsets();
void progress(std::string const& suffix);
void progress_comp(bool trans);
void write_skill_anim();
void write_item_icons();
void write_all_item_icons();

uint8 zeroes[65536];

class Mem32;
class MemValue {
public:
  MemValue(uint8 const* ptr, uint32 offset) : ptr_(ptr), offset_(offset) {}
  MemValue operator[](uint32 offset) const {
    return MemValue(ptr_, offset_ + offset);
  }

  uint8 get() const {
    return ptr_[offset_];
  }
  uint16 get16() const {
    return *(uint16*) (ptr_ + offset_);
  }
  uint32 get32() const {
    return *(uint32*) (ptr_ + offset_);
  }
  uint64 get64() const {
    return *(uint64*) (ptr_ + offset_);
  }
  uint32 offset() const {
    return offset_;
  }
  MemValue ptr() const {
    return MemValue(ptr_, get32());
  }
  char const* string() const {
    return (char*) (ptr_ + offset_);
  }
  char const* stringPtr() const {
    return (char*) (ptr_ + get32());
  }
private:
  uint8 const* ptr_;
  uint32 offset_;
};

class Mem32 {
public:
  Mem32(File& file, uint32 size = 0x02000000)
    : size_(size)
  {
    ptr_ = new uint8[size];
    file.read(ptr_, size);
  }
  Mem32(std::string const& path, uint32 size = 0x02000000)
    : Mem32(File(path))
  {}
  ~Mem32() {
    delete[] ptr_;
  }
  MemValue operator[](uint32 offset) const {
    return MemValue(ptr_, offset);
  }

  bool valid(uint32 offset) const {
    return offset < size_;
  }
  bool valid(MemValue value) const {
    return valid(value.offset());
  }
private:
  uint8* ptr_;
  uint32 size_;
};

bool goodString(char const* str) {
  uint32 len = 0;
  while (len < 64 && str[len]) {
    if (!isalnum((uint8) str[len])) return false;
    ++len;
  }
  return (len > 0 && len < 64);
}

Map<uint32> numTypes;
std::map<uint32, std::string> typeMap;

std::string const& DiscoverType(MemValue data, json::Value& root) {
  if (typeMap.count(data.offset())) {
    return typeMap[data.offset()];
  }
  auto& name = typeMap[data.offset()];
  name = data[4].stringPtr();
  uint32& index = numTypes[name];
  if (index) name.append(fmtstring("_%d", index));
  ++index;

  auto& res = root[name];
  auto fields = data[12].ptr();
  if (fields.offset() == 0) {
    res["basic"] = true;
    return name;
  }
  while (fields.get32() != 1) {
    auto type = DiscoverType(fields[4].ptr(), root);
    if (type == "DT_NULL") break;
    auto& dst = res["fields"].append(json::Value::tObject);
    auto name = fields.stringPtr();
    if (*name) dst["name"] = name;
    dst["type"] = type;
    dst["offset"] = fields[8].get32();
    if (fields[0x10].get32()) dst["min"] = fields[0x10].get32();
    if (fields[0x14].get32()) dst["max"] = fields[0x14].get32();
    if (fields[0x18].get32() != 1) dst["flags"] = fields[0x18].get32();
    auto sub = DiscoverType(fields[0x1C].ptr(), root);
    if (sub != "DT_NULL") dst["subtype"] = sub;
    if (fields[0x20].get32()) dst["varOffset"] = (int) fields[0x20].get32();
    if (fields[0x24].get32() != -1) dst["arrayLength"] = (int) fields[0x24].get32();
    if (fields[0x28].get32()) dst["arrayLengthOffset"] = (int) fields[0x28].get32();
    if (fields[0x2C].get16()) dst["encodedBits"] = fields[0x2C].get16();
    if (fields[0x2E].get16()) dst["encodedBits2"] = fields[0x2E].get16();
    if (fields[0x30].get32() != -1) dst["snoType"] = (int) fields[0x30].get32();
    if (fields[0x34].get32()) dst["tagMapRelated"] = (int) fields[0x34].get32();
    auto enumFields = fields[0x38].ptr();
    if (enumFields.offset()) {
      auto& ef = dst["enumFields"];
      while (enumFields[4].get32()) {
        ef[fmtstring("%d", enumFields.get32())] = enumFields[4].stringPtr();
        enumFields = enumFields[8];
      }
    }
    if (fields[0x3C].get32()) dst["flagIndex"] = (int) fields[0x3C].get32();
    if (fields[0x48].get32() != -1) dst["dspIndex"] = (int) fields[0x48].get32();
    fields = fields[140];
  }

  return name;
}

std::string _sanitize(std::string const& str) {
  std::string res;
  for (auto chr : str) {
    if (chr != '"') res += chr;
  }
  return res;
}
void str_comp() {
  SnoSysLoader cnLoader(path::root());
  for (auto& en : SnoLoader::All<StringList>()) {
    if (en.name().find("Conv_") != std::string::npos || en.name().find("Quest_") != std::string::npos) continue;
    SnoFile<StringList> cn(en.name(), &cnLoader);
    if (!cn) continue;
    Map<std::string> nameCn;
    for (auto& it : cn->x10_StringTableEntries) {
      nameCn[it.x00_Text.text()] = it.x10_Text.text();
    }
    File output("strings/" + en.name() + ".csv", "wb");
    output.putc(0xEF);
    output.putc(0xBB);
    output.putc(0xBF);
    for (auto& it : en->x10_StringTableEntries) {
      auto ci = nameCn.find(it.x00_Text.text());
      if (ci != nameCn.end()) {
        output.printf("\"%s\",\"%s\"\r\n", _sanitize(it.x10_Text.text()).c_str(), _sanitize(ci->second).c_str());
      }
    }
  }
}

void copyFile(File& dst, File& src) {
  static uint8 buf[65536];
  while (uint32 count = src.read(buf, sizeof buf)) {
    dst.write(buf, count);
  }
}
void powertags();

void itemPowerFilter();
void fixItems();
void fixSets2();
void FormatLocale(std::string const& dest, int flags = 0);
void CompareLocale();

void checkPower(uint32 id, json::Value& dst, uint32 dexp) {
  auto* tag = PowerTags::getraw(id);
  uint32 dual = tag->getint("AffectedByDualWield");
  uint32 mhonly = tag->getint("Uses Mainhand Only");
  if (mhonly || dual != dexp) {
    auto list = Strings::list("Powers");
    auto& cur = dst[list.getfmt("%s_name", Power::name(id))];
    if (mhonly) cur["Uses Mainhand Only"] = mhonly;
    if (dual != dexp) cur["AffectedByDualWield"] = dual;
  }
}

std::string strdiffns(std::string const& lhs, std::string const& rhs) {
  size_t n = lhs.size();
  size_t m = rhs.size();
  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
  for (int i = n - 1; i >= 0; --i) {
    for (int j = m - 1; j >= 0; --j) {
      if (lhs[i] == rhs[j]) {
        dp[i][j] = dp[i + 1][j + 1] + 1;
      } else {
        dp[i][j] = std::max(dp[i + 1][j], dp[i][j + 1]);
      }
    }
  }
  size_t ci = 0, cj = 0;
  size_t pi = 0, pj = 0;
  std::string result;
  while (ci < n && cj < m) {
    if (lhs[ci] == rhs[cj]) {
      if (pi < ci) {
        result.append("<span class=\"bg-red\">");
        result.append(lhs.substr(pi, ci - pi));
        result.append("</span>");
      }
      if (pj < cj) {
        result.append("<span class=\"bg-green\">");
        result.append(rhs.substr(pj, cj - pj));
        result.append("</span>");
      }
      result.push_back(lhs[ci]);
      pi = ++ci;
      pj = ++cj;
    } else if (dp[ci][cj] == dp[ci + 1][cj]) {
      ++ci;
    } else {
      ++cj;
    }
  }
  if (pi < n) {
    result.append("<span class=\"bg-red\">");
    result.append(lhs.substr(pi));
    result.append("</span>");
  }
  if (pj < m) {
    result.append("<span class=\"bg-green\">");
    result.append(rhs.substr(pj));
    result.append("</span>");
  }
  return result;
}

typedef std::map<std::string, std::string> StrMap;
StrMap parse(std::string const& str) {
  size_t left = str.find('[') + 1;
  size_t right = str.find(']');
  auto parts = split(str.substr(left, right - left), ';');
  StrMap res;
  for (auto& sub : parts) {
    size_t eq = sub.find('=');
    if (eq == std::string::npos) continue;
    res[sub.substr(0, eq)] = sub.substr(eq + 1);
  }
  return res;
}

std::string mapdiff(StrMap const& lhs, StrMap const& rhs) {
  std::string result;
  std::set<std::string> keys;
  for (auto& kv : lhs) keys.insert(kv.first);
  for (auto& kv : rhs) keys.insert(kv.first);
  for (auto& key : keys) {
    auto lit = lhs.find(key), rit = rhs.find(key);
    std::string lstr = (lit == lhs.end() ? "" : lit->second);
    std::string rstr = (rit == rhs.end() ? "" : rit->second);
    if (lstr != rstr) {
      if (!result.empty()) result.push_back(';');
      result.append(key);
      result.push_back('=');
      if (!lstr.empty()) {
        result.append("<span class=\"bg-red\">");
        result.append(lstr);
        result.append("</span>");
      }
      if (!rstr.empty()) {
        result.append("<span class=\"bg-green\">");
        result.append(rstr);
        result.append("</span>");
      }
    }
  }
  return result;
}

void dumpTags();

#pragma warning(disable: 4005)
#ifdef _WIN64
#include "CascLib64/CascLib.h"
#else
#include "CascLib/CascLib.h"
#endif

#include "affixes.h"
#include "ngdp.h"
#include "checksum.h"
#include "poe.h"
#include "textures.h"
#include <unordered_set>
File http_get(std::string const& url);

std::string get_version();

int do_main() {
  auto builds = SnoCdnLoader::builds();
  std::map<istring, std::map<std::string, int>> sizes;
  for (auto const& build : builds) {
    std::string name = SnoCdnLoader::buildinfo(build)["build-name"];
    json::Value val;
    json::parse(File("cdt" / name + ".js"), val);
    for (auto const& kv : val.getMap()) {
      sizes[kv.first][name] = kv.second.getInteger();
    }
  }
  json::Value out;
  for (auto const& kv : sizes) {
    int prev = 0;
    json::Value& dst = out[kv.first];
    for (auto const& kvi : kv.second) {
      if (kvi.second != prev) {
        prev = kvi.second;
        dst[kvi.first] = kvi.second;
      }
    }
  }
  json::write(File("cdt.js", "wb"), out);
  //  if (SnoCdnLoader::buildinfo(build)["build-name"] > "30687") continue;
  //  SnoCdnLoader casc(build, "enUS");
  //  File cdt = casc.load("ComplexTypeDescriptorSizes.dat");
  //  if (!cdt || cdt.read32() != 0xAABBCCDD) continue;
  //  size_t count = cdt.size() / (1024 + 4 + 64);
  //  json::Value vers;
  //  for (size_t i = 0; i < count; ++i) {
  //    char name[64];
  //    cdt.read(name, sizeof name);
  //    vers[name] = cdt.read32();
  //    cdt.seek(1024, SEEK_CUR);
  //  }
  //  json::write(File(fmtstring("cdt/%s.js", casc.buildinfo().at("build-name").c_str()), "wb"), vers);
  //  //auto install = casc.install();
  //  //File(fmtstring("exe/%s.exe", casc.buildinfo().at("build-name")), "wb").copy(casc.load(install["Diablo III.exe"]));
  //}
  //return 0;
  return 0;

#ifdef MODELVIEWER
  ViewModels();
  return 0;
#endif
  auto ver = get_version();
  Logger::log(ver.c_str());
  return 0;
  SnoManager::clear();
  //powertags();
  //progress("_ptr.js");
  //SkillTips::dump(true);
  //SnoManager::clear();
  //dump_data("_ptr.js");
  //return 0;
  dump_data("_live.js");
  //casc.dump<GameBalance>();
  //SkillTips::dump(true);
  make_menu();
  make_diffs();
  return 0;
  json::Value sin;
  json::parse(File("kadala3.js"), sin, json::mJS);
  for (auto& kv : sin.getMap()) {
    sin[kv.first]["name"] = Strings::get("Items", kv.first);
  }
  json::write(File("kadala2.js", "w"), sin, json::mJS);
  return 0;
  json::Value sn, sp, sr, isrc, iic, ilvl;
  json::parse(File("skill_names.js"), sn, json::mJS);
  json::parse(File("skill_powers.js"), sp, json::mJS);
  for (auto& kv : sn["skills"].getMap()) {
    auto& dc = sr[kv.first];
    for (auto& ki : kv.second.getMap()) {
      std::string pow = sp["skills"][kv.first][ki.second["name"].getString()].getString();
      PowerTag* tag = PowerTags::get(pow);
      if (!tag) {
        Logger::log("power not found for %s", ki.first.c_str());
        continue;
      }
      AttributeValue vals[6];
      AttributeMap attr = GameAffixes::defaultMap();
      vals[0] = tag->get("Attack Radius", attr);
      bool varies = false;
      for (int i = 1; i < 6; ++i) {
        AttributeMap rattr = attr;
        rattr[fmtstring("Rune_%c", 'A' + (i - 1))] = 1;
        vals[i] = tag->get("Attack Radius", rattr);
        if (vals[i].max != vals[0].max) varies = true;
      }
      if (varies) {
        auto& dst = dc[ki.first];
        dst["x"] = vals[0].max;
        for (int i = 1; i < 6; ++i) {
          dst[fmtstring("%c", 'a' + (i - 1))] = vals[i].max;
        }
      } else {
        dc[ki.first] = vals[0].max;
      }
    }
  }
  json::write(File("ranges.js", "w"), sr, json::mJS);
  return 0;
  json::parse(File("items.js"), isrc, json::mJS);
  json::parse(File("item_icons.js"), iic, json::mJS);
  for (auto& iid : isrc["itemById"].getMap()) {
    auto* it = ItemLibrary::get(iid.first);
    if (!it || !it->x3C4) continue;
    auto& dst = ilvl[it->x000_Text];
    dst["name"] = iid.second["name"];
    dst["type"] = iid.second["type"];
    if (iic.has(iid.first)) {
      dst["icon"] = iic[iid.first][0];
    }
    dst["level"] = it->x14C;
    dst["weight"] = it->x3C4;
    if (it->x3C8) dst["demonhunter"] = true;
    if (it->x3CC) dst["barbarian"] = true;
    if (it->x3D0) dst["wizard"] = true;
    if (it->x3D4) dst["witchdoctor"] = true;
    if (it->x3D8) dst["monk"] = true;
    if (it->x3DC) dst["crusader"] = true;
    if (it->x110_Bit18) dst["hardcore"] = true;

    File data = http_get(
      fmtstring("https://us.api.battle.net/d3/data/item/%s?apikey=nzshbg67hcyh4epzjdyc4wrkxx4xcpsp", it->x000_Text));
    if (data) {
      try {
        json::Value val;
        json::parse(data, val);
        dst["tooltip"] = val["tooltipParams"];
      } catch (...) {
      }
      Sleep(500);
    }
    if (!dst.has("tooltip")) {
      Logger::log("no tooltip for %s", iid.second["name"].getString().c_str());
    }
  }
  json::write(File("kadala.js", "w"), ilvl, json::mJS);
  return 0;
  ////itemPowerFilter();
  //SkillTips::dump(true);
  //progress("_live.js");
  //progress_comp(false);
  //casc.dump<GameBalance>();
  //Archive dsticons;
  //dsticons.load(File("item_icons.wgz"), false);
  //Image runes(64, 64 * 33);
  //for (int i = 1; i <= 33; ++i) {
  //  std::string name = fmtstring("Rune%02dIcon", i);
  //  uint32 id = HashName(name);
  //  Image image = GameTextures::get(id);
  //  runes.blt(0, 64 * (i - 1), image);
  //}
  //runes.resize(24, 24 * 33).write("runes.png");
  //json::write(File("runes.js", "w"), rlist, json::mJS);
  //return 0;
  //File dst("HandicapLevels.csv", "w");
  //for (auto& gmb : casc.all<GameBalance>()) {
  //  for (auto& tl : gmb->x138_HandicapLevelTable) {
  //    //dst.printf("%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%lld,%f,%f\n",
  //    //  tl.x00, tl.x04, tl.x08, tl.x0C,
  //    //  tl.x10, tl.x14, tl.x18, tl.x1C,
  //    //  tl.x20, tl.x24, tl.x28, tl.x30, tl.x34);
  //    dst.printf("%f,%f,%f,%f,%f,%f,%d,%d\n",
  //      tl.x00, tl.x04, tl.x08, tl.x0C,
  //      tl.x10, tl.x14, tl.x18, tl.x1C);
  //  }
  //}
  //return 0;
  FormatLocale("locale/en", 3);

  std::vector<SnoLoader*> loaders;

  json::Value langNames;

  for (auto& locale : std::vector<std::string>{
    "ruRU", "zhCN", "zhTW", "plPL", "deDE", "frFR", "esES", "koKR", "ptBR", "itIT"
  }) {
    Logger::log(locale.c_str());
    if (locale == "zhCN"/* || locale == "ptBR" || locale == "itIT"*/) {
      loaders.push_back(new SnoCascLoader(R"(E:\Games\D3Cn)", locale));
    } else {
      loaders.push_back(new SnoCascLoader(path::casc(), locale));
    }
    Strings::setLoader(loaders.back());
    langNames[locale] = Strings::get("GameOptions", "Label_" + locale);
    FormatLocale("locale" / locale, 2);
  }
  for (auto* loader : loaders) {
    delete loader;
  }

  json::write(File("langnames.js", "w"), langNames);

  return 0;

  SnoCascLoader ruLoader(path::casc(), "ruRU");
  SnoSysLoader cnLoader(path::root() / "zhCN");
  SnoSysLoader twLoader(path::root() / "zhTW");
  SnoSysLoader plLoader(path::root() / "plPL");
  SnoSysLoader deLoader(path::root() / "deDE");
  SnoSysLoader frLoader(path::root() / "frFR");
  SnoSysLoader esLoader(path::root() / "esES");
  SnoSysLoader krLoader(path::root() / "koKR");

  int mode = 2;
  Strings::setLoader(&ruLoader);
  FormatLocale("locale/ruRU", mode);
  //Strings::setLoader(&cnLoader);
  //FormatLocale("locale/zhCN", mode);
  Strings::setLoader(&twLoader);
  FormatLocale("locale/zhTW", mode);
  //Strings::setLoader(&deLoader);
  //FormatLocale("locale/deDE", mode);
  //Strings::setLoader(&plLoader);
  //FormatLocale("locale/plPL", mode);
  //Strings::setLoader(&frLoader);
  //FormatLocale("locale/frFR", mode);
  //Strings::setLoader(&esLoader);
  //FormatLocale("locale/esES", mode);
  //Strings::setLoader(&krLoader);
  //FormatLocale("locale/koKR", 2);

  return 0;
}

#include <shlobj.h>
bool browseForFolder(wchar_t const* prompt, std::string& result) {
  IFileDialog* pfd;
  if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd)))) {
    return false;
  }
  DWORD dwOptions;
  if (SUCCEEDED(pfd->GetOptions(&dwOptions))) {
    pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
  }
  pfd->SetTitle(prompt);
  if (FAILED(pfd->Show(NULL))) {
    pfd->Release();
    return false;
  }
  IShellItem* psi;
  if (FAILED(pfd->GetResult(&psi))) {
    pfd->Release();
    return false;
  }
  wchar_t* str;
  if (FAILED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &str)) || !str) {
    psi->Release();
    pfd->Release();
    return false;
  }
  std::wstring ws(str);
  result = std::string(ws.begin(), ws.end());
  psi->Release();
  pfd->Release();
  return true;
}

int main() {
  std::setlocale(LC_ALL, "eu_US.UTF-8");

  INITCOMMONCONTROLSEX iccex;
  iccex.dwSize = sizeof iccex;
  iccex.dwICC = ICC_STANDARD_CLASSES | ICC_PROGRESS_CLASS |
    ICC_BAR_CLASSES | ICC_TREEVIEW_CLASSES | ICC_LISTVIEW_CLASSES |
    ICC_TAB_CLASSES | ICC_UPDOWN_CLASS | ICC_DATE_CLASSES;
  InitCommonControlsEx(&iccex);
  OleInitialize(NULL);

#ifdef MODELVIEWER
  char myPath[512];
  //GetModuleFileName(NULL, myPath, 512);
  GetCurrentDirectory(sizeof myPath, myPath);
  path::roots.push_back(myPath);// path::path(myPath));

  std::string line;
  {
    File fpath(path::roots[0] / "path.txt", "rt");
    if (fpath) fpath.getline(line);
  }
  uint32 attr = GetFileAttributes(line.c_str());
  if (attr == INVALID_FILE_ATTRIBUTES || !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
    if (!browseForFolder(L"Locate D3 Data", line)) return 0;
    File fpath(path::roots[0] / "path.txt", "wt");
    fpath.printf("%s", line.c_str());
  }
  path::cascs.push_back(line);
#endif

  try {
    return do_main();
  } catch (Exception& ex) {
    fprintf(stderr, "Exception: %s\n", ex.what());
    fprintf(stderr, "Press any key to continue . . .\n");
    getch();
    return 1;
  }
}
