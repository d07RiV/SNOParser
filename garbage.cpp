#include <stdio.h>
#include <iostream>
#include <clocale>
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
#include "types/Monster.h"
#include "types/StringList.h"
#include "types/AnimSet.h"
#include "regexp.h"
#include "translations.h"
#include "description.h"
#include "miner.h"
#include "powertag.h"
#include "itemlib.h"
#include "affixes.h"

#include <map>
#include <vector>
#include <algorithm>
#include <stdarg.h>
using namespace std;

//pair<uint32, pair<uint32, float>> GetFPA(char const* name);

string mprintf(char const* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  string buf;
  buf.resize(_vscprintf(fmt, ap));
  vsprintf(&buf[0], fmt, ap);
  return buf;
}

vector<pair<float, string>> speeds;

void GetSpeeds() {
  for (float spd : {1.0f, /*1.05f,*/ 1.1f, 1.15f, 1.2f, 1.3f, 1.4f, 1.5f, /*1.6f,*/}) {
    for (int wpnias : {0, 5, 6, 7}) {
      for (int ias_fct = 0; ias_fct <= 600; ++ias_fct) {
        float ias_pct = float(ias_fct / 5) + float(ias_fct % 5) * 0.2;
        float ias = 1 + 0.01f * ias_pct;
        float aps = spd * ias;
        if (wpnias) {
          float wias = 1 + 0.01f * wpnias;
          speeds.emplace_back(aps * wias, mprintf("%g (+%d%%) @ %g%%", spd, wpnias, ias_pct));
        } else {
          speeds.emplace_back(aps, mprintf("%g @ %g%%", spd, ias_pct));
        }
      }
    }
  }
  sort(speeds.begin(), speeds.end());
}

//void CheckSpeeds() {
//  GetSpeeds();
//  for (auto& speed : speeds) {
//    printf("%g = %s\n", speed.first, speed.second.c_str());
//  }
//  for (auto name : {"Wizard_Female_1HS_Buff_01",
//                    "Wizard_Female_1HS_SpellCast_Summon",
//                    "Wizard_Female_1HS_Attack_01_spell",
//                    "Wizard_Female_1HS_SpellCast_Directed_01",
//                    "Wizard_Female_1HS_SpellCast_AOE_02",
//                    "Wizard_Female_HTH_SpellCast_AOE",
//                    "Wizard_Female_HTH_X1_ArcaneOrb_Throw"}) {
//    auto fpa = GetFPA((string(name) + Anim::ext()).c_str());
//    uint32 frames = fpa.second.first;
//    float velocity = fpa.second.second;
//    float base_riv = float(frames - 1) / velocity;
//    float base_doly = 60.0f * float(frames - 1) / float(frames);
//    for (auto& speed : speeds) {
//      int riv = ceil(base_riv / speed.first);
//      int doly = ceil(base_doly / speed.first);
//      if (riv != doly) {
//        printf("Mismatch for %s:\n", name);
//        printf("Frames: %d, velocity: %f\n", frames, velocity);
//        printf("Speed: %g=%s\n", speed.first, speed.second.c_str());
//        printf("Riv: %d\n", riv);
//        printf("Doly: %d\n", doly);
//        printf("\n");
//      }
//    }
//  }
//}

Dictionary reverse(Dictionary const& dict) {
  Dictionary out;
  for (auto& kv : dict) {
    out.emplace(kv.second, kv.first);
  }
  return out;
}
void dumpKits(std::map<std::string, std::string>& strings);

std::string translate(std::string const& orig, std::string const& from, std::string const& to) {
  static re::Prog orig_parser("\\+?%(d|(\\.[0-9])?f)|\\+?[0-9]+(\\.[0-9]+)?");
  static re::Prog fmt_parser("\\+?\\{([^}]+)\\}|\\+?\\[([^\\]]+)\\]|\\+?[0-9]+(\\.[0-9]+)?|%|\\n");
  std::vector<std::string> opts = orig_parser.findAll(orig);
  uint32 index = 0;
  Dictionary ref;
  fmt_parser.findAll(from, [&opts, &index, &ref](re::Match const& match) {
    std::string whole = match.group(0);
    if (whole[0] == '+') whole.erase(0, 1);
    if (whole == "%") {
      return;
    } else if (whole == "\n") {
      return;
    } else if (whole[0] == '{') {
      if (whole == "{icon:bullet}") return;
      if (whole[1] == 'c') return;
      if (whole[1] == '/') return;
      if (index >= opts.size()) return;
      ref[whole] = opts[index++];
    } else if (whole[0] == '[') {
      if (index >= opts.size()) return;
      ref[whole] = opts[index++];
    } else {
      index++;
    }
  });
  return fmt_parser.replace(to, [&ref](re::Match const& match) {
    std::string whole = match.group(0);
    if (whole[0] == '+') whole.erase(0, 1);
    if (whole == "%") {
      return std::string("%%");
    } else if (whole == "\n") {
      return std::string("\r\n");
    } else if (whole[0] == '{') {
      if (whole == "{icon:bullet}") return std::string();
      if (whole[1] == 'c') return std::string();
      if (whole[1] == '/') return std::string();
      return ref[whole];
    } else if (whole[0] == '[') {
      return ref[whole];
    } else {
      return match.group(0);
    }
  });
}

SnoSysLoader& euLoader() {
  static SnoSysLoader loader(path::work() / "Eu");
  return loader;
}
SnoSysLoader& cnLoader() {
  static SnoSysLoader loader(path::root());
  return loader;
}

#include "types/gamebalance.h"
void fixItems(json::Value& items) {
  auto itemNames = Strings::list("Items", &cnLoader());
  auto powerDesc = Strings::list("ItemPassivePowerDescriptions", &cnLoader());
  auto powerDescEu = Strings::list("ItemPassivePowerDescriptions");
  Dictionary itemPowers;
  Dictionary itemPowersEu;
  for (auto& value : SnoLoader::All<GameBalance>()) {
    for (auto& item : value->x028_Items) {
      for (auto& attr : item.x1F8_AttributeSpecifiers) {
        if (attr.x00_Type == fixAttrId(1270, true)) {
          char const* power = Power::name(attr.x04_Param);
          if (power) {
            itemPowers[item.x000_Text] = powerDesc[power];
            itemPowersEu[item.x000_Text] = powerDescEu[power];
          }
        }
      }
    }
  }

  for (auto& it = items.begin(); it != items.end(); ++it) {
    if (itemNames.has(it.key())) {
      it->insert("name", itemNames[it.key()]);
    }
    if (it->has("required") && itemPowers.count(it.key())) {
      auto& format = (*it)["required"]["custom"]["format"];
      format.setString(translate(format.getString(), itemPowersEu[it.key()], itemPowers[it.key()]));
    }
  }
}
void fixItems() {
  json::Value items;
  std::string func;
  json::parse(File(path::work() / "locale" / "items.js"), items, json::mJSCall, &func, true);
  fixItems(items["itemById"]);
  json::write(File(path::work() / "locale" / "items.js", "w"), items, json::mJSCall, func.c_str());
}

void fixSets(json::Value& sets) {
  struct SetDesc {
    std::vector<uint32> bonuses[8];
  };
  Map<SetDesc> setBonuses;
  auto setNames = Strings::list("ItemSets", &cnLoader());
  Dictionary set2id = reverse(readStrings("ItemSets", &cnLoader()));
  SnoFile<GameBalance> gmb("SetItemBonuses");
  for (auto& bonus : gmb->x168_SetItemBonusTable) {
    std::vector<std::string> sub;
    if (re::Prog("([^(]*)( \\([0-9]+\\))?").match(bonus.x000_Text, &sub)) {
      std::string id = sub[1];
      for (auto& attr : bonus.x110_AttributeSpecifiers) {
        if (attr.x00_Type == 1265) {
          setBonuses[id].bonuses[bonus.x10C].push_back(attr.x04_Param);
        }
      }
    }
  }
  auto powerDesc = Strings::list("ItemPassivePowerDescriptions", &cnLoader());
  auto powerDescEu = Strings::list("ItemPassivePowerDescriptions");

  for (auto& set : sets) {
    std::string name = set["name"].getString();
    if (set2id.count(name)) {
      std::string id = set2id[name];
      if (setNames.has(id)) {
        set["name"] = setNames[id];
      }
      if (setBonuses.count(id)) {
        SetDesc& bonus = setBonuses[id];
        for (auto& it = set["bonuses"].begin(); it != set["bonuses"].end(); ++it) {
          int pcs = static_cast<int>(it.key()[0] - '0');
          uint32 index = 0;
          for (auto& pow : *it) {
            if (pow.has("format") && index < bonus.bonuses[pcs].size()) {
              uint32 power = bonus.bonuses[pcs][index++];
              char const* pow_name = Power::name(power);
              if (pow_name && powerDesc.has(pow_name)) {
                pow["format"] = translate(pow["format"].getString(), powerDescEu[pow_name], powerDesc[pow_name]);
              }
            }
          }
        }
      }
    }
  }
}
void fixSets() {
  json::Value sets;
  std::string func;
  json::parse(File(path::work() / "locale" / "itemsets.js"), sets, json::mJSCall, &func, true);
  fixSets(sets["itemSets"]);
  json::write(File(path::work() / "locale" / "itemsets.js", "w"), sets, json::mJSCall, func.c_str());
}

void fixStats(json::Value& stats) {
  json::Value src;
  json::parse(File(path::work() / "stats_src.js"), src, json::mJS, nullptr, true);
  Dictionary desc = readStrings("AttributeDescriptions");
  Dictionary descEu = readStrings("AttributeDescriptions", &euLoader());

  for (auto& it = stats.begin(); it != stats.end(); ++it) {
    auto& stat = *it;
    if (src.has(it.key())) {
      auto& src_stat = src[it.key()];
      if (src_stat.has("id") && desc.count(src_stat["id"].getString())) {
        std::string id = src_stat["id"].getString();
        std::string format;
        if (stat.has("format")) {
          format = stat["format"].getString();
        } else {
          int args = 1;
          if (src_stat.has("args")) {
            args = src_stat["args"].getInteger();
          }
          format = "+%d";
          for (int i = 1; i < args; ++i) {
            format += "-%d";
          }
          format += " " + src_stat["name"].getString();
        }
        stat["format"].setString(translate(format, descEu[id], desc[id]));
      }
    }
  }
}
void fixStats() {
  json::Value stats;
  std::string func;
  json::parse(File(path::work() / "locale" / "stats.js"), stats, json::mJSCall, &func, true);
  fixStats(stats["stats"]);
  json::write(File(path::work() / "locale" / "stats.js", "w"), stats, json::mJSCall, func.c_str());
}

#include "types/skillkit.h"
void fixSkills(json::Value& skills) {
  Dictionary name_cn = readStrings("Powers");
  Dictionary name_eu = readStrings("Powers", &euLoader());
  Dictionary desc = readStrings("AttributeDescriptions");
  Dictionary name2pow;
  for (auto& kit : SnoLoader::All<SkillKit>()) {
    for (auto& trait : kit->x10_TraitEntries) {
      std::string power = trait.x00_PowerSno.name();
      if (name_eu.count(power + "_name")) {
        name2pow[name_eu[power + "_name"]] = power;
      }
    }
    for (auto& skill : kit->x20_ActiveSkillEntries) {
      std::string power = skill.x00_PowerSno.name();
      if (name_eu.count(power + "_name")) {
        name2pow[name_eu[power + "_name"]] = power;
      }
    }
  }

  for (auto& cat : skills) {
    for (auto& cls : cat) {
      for (auto& skill : cls) {
        std::string name = skill["name"].getString();
        if (name2pow.count(name)) {
          std::string power = name2pow[name];
          skill["name"] = name_cn[power + "_name"];
          if (skill.has("runes")) {
            for (char rune = 'a'; rune <= 'e'; ++rune) {
              std::string rune_id;
              rune_id.push_back(rune);
              std::string rune_idx = "NameRune_";
              rune_idx.push_back(rune - 'a' + 'A');
              rune_idx += "#" + power;
              if (desc.count(rune_idx)) {
                skill["runes"][rune_id] = desc[rune_idx];
              }
            }
          }
        }
      }
    }
  }
}
void fixSkills() {
  json::Value skills;
  std::string func;
  json::parse(File(path::work() / "locale" / "skills.js"), skills, json::mJSCall, &func, true);
  fixSkills(skills);
  json::write(File(path::work() / "locale" / "skills.js", "w"), skills, json::mJSCall, func.c_str());
}

void fixGems(json::Value& gems) {
  json::Value src;
  json::parse(File(path::work() / "gems.js"), src, json::mJS, nullptr, true);
  Dictionary itemNames = readStrings("Items");
  Map<std::pair<uint32, uint32>> itemInfo;
  for (auto& value : SnoLoader::All<GameBalance>()) {
    for (auto& item : value->x028_Items) {
      std::string id = item.x000_Text;
      if (item.x4B8_AttributeSpecifier.x00_Type == 1265) {
        itemInfo[id].first = item.x4B8_AttributeSpecifier.x04_Param;
      }
      if (item.x4D0_AttributeSpecifier.x00_Type == 1265) {
        itemInfo[id].second = item.x4D0_AttributeSpecifier.x04_Param;
      }
    }
  }
  Dictionary powerDesc = readStrings("ItemPassivePowerDescriptions");
  Dictionary powerDescEu = readStrings("ItemPassivePowerDescriptions", &euLoader());
  for (auto& it = gems.begin(); it != gems.end(); ++it) {
    if (!it->has("effects")) continue;
    std::string id = src["legendaryGems"][it.key()]["id"].getString();
    (*it)["name"] = itemNames[id];
    auto& fx = (*it)["effects"];
    if (fx.has("0") && fx["0"].has("format")) {
      std::string power;
      char const* power_sno = Power::name(itemInfo[id].first);
      if (power_sno) {
        power = power_sno;
      } else {
        power = "ItemPassive_" + id;
      }
      if (powerDesc.count(power)) {
        fx["0"]["format"] = translate(fx["0"]["format"].getString(), powerDescEu[power], powerDesc[power]);
      }
    }
    if (fx.has("1") && fx["1"].has("format")) {
      std::string power;
      char const* power_sno = Power::name(itemInfo[id].second);
      if (power_sno) {
        power = power_sno;
      } else {
        power = "ItemPassive_" + re::Prog("_([0-9][0-9][0-9])_").replace(id, "_\\1U_");
      }
      if (powerDesc.count(power)) {
        fx["1"]["format"] = translate(fx["1"]["format"].getString(), powerDescEu[power], powerDesc[power]);
      }
    }
  }
}
void fixGems() {
  json::Value gems;
  std::string func;
  json::parse(File(path::work() / "locale" / "gems.js"), gems, json::mJSCall, &func, true);
  fixGems(gems["legendaryGems"]);
  json::write(File(path::work() / "locale" / "gems.js", "w"), gems, json::mJSCall, func.c_str());
}

void fixSimBuffs(json::Value& src, json::Value& dst) {
  json::Value skills_cn, skills_eu;
  json::parse(File(path::work() / "locale" / "skills.js"), skills_cn, json::mJSCall);
  json::parse(File(path::work() / "skills.js"), skills_eu, json::mJSCall);
  json::Value gems_cn, gems_eu;
  json::parse(File(path::work() / "locale" / "gems.js"), gems_cn, json::mJSCall);
  json::parse(File(path::work() / "gems.js"), gems_eu, json::mJSCall);
  json::Value sets_cn, sets_eu;
  json::parse(File(path::work() / "locale" / "itemsets.js"), sets_cn, json::mJSCall);
  json::parse(File(path::work() / "itemsets.js"), sets_eu, json::mJSCall);
  json::Value items_cn, items_eu;
  json::parse(File(path::work() / "locale" / "items.js"), items_cn, json::mJSCall);
  json::parse(File(path::work() / "items.js"), items_eu, json::mJSCall);

  json::Value items_map;
  json::parse(File(path::work() / "itemmap.js"), items_map, json::mJS);
  Dictionary id2item;
  for (auto& item : items_map["itemById"].getMap()) {
    id2item[item.second["required"]["custom"]["id"].getString()] = item.first;
  }

  for (auto& it : src.getMap()) {
    auto& buff = it.second;
    std::string name = buff["name"].getString();
    std::string cls;
    if (buff.has("classes")) {
      cls = buff["classes"][0].getString();
    }
    std::string eu, cn;
    if (buff.has("skill")) {
      std::string id = buff["skill"][0].getString();
      if (skills_cn["skills"][cls].has(id)) {
        auto& skill_cn = skills_cn["skills"][cls][id];
        auto& skill_eu = skills_eu["skills"][cls][id];
        cn = skill_cn["name"].getString();
        eu = skill_eu["name"].getString();
        if (buff["skill"].length() > 1) {
          std::string rune = buff["skill"][1].getString();
          cn += "-" + skill_cn["runes"][rune].getString();
          eu += " - " + skill_eu["runes"][rune].getString();
        }
      }
    } else if (buff.has("passive")) {
      std::string id = buff["passive"].getString();
      if (skills_cn["passives"][cls].has(id)) {
        cn = skills_cn["passives"][cls][id]["name"].getString();
        eu = skills_eu["passives"][cls][id]["name"].getString();
      }
    } else if (buff.has("gem")) {
      std::string id = buff["gem"][0].getString();
      if (gems_cn["legendaryGems"].has(id)) {
        cn = gems_cn["legendaryGems"][id]["name"].getString();
        eu = gems_eu["legendaryGems"][id]["name"].getString();
      }
    } else if (buff.has("set")) {
      std::string id = buff["set"][0].getString();
      if (sets_cn["itemSets"].has(id)) {
        cn = sets_cn["itemSets"][id]["name"].getString();
        eu = sets_eu["itemSets"][id]["name"].getString();
      }
    } else if (buff.has("stat")) {
      std::string id = buff["stat"].getString();
      if (id2item.count(id)) id = id2item[id];
      if (items_cn["itemById"].has(id)) {
        cn = items_cn["itemById"][id]["name"].getString();
        eu = items_eu["itemById"][id]["name"].getString();
      }
    }
    if (!eu.empty() && !cn.empty()) {
      if (name == eu) {
        name = cn;
      } else if (name.length() > eu.length() && name.substr(0, eu.length()) == eu) {
        name = cn + name.substr(eu.length());
      }
    }
    dst[it.first]["name"] = name;
    dst[it.first]["category"] = buff["category"];
  }
}
void fixSimBuffs() {
  json::Value buffs, nbuffs;
  json::parse(File(path::work() / "simbuffs.js"), buffs, json::mJS);
  nbuffs["status"] = buffs["status"];
  fixSimBuffs(buffs["buffs"], nbuffs["buffs"]);
  json::write(File(path::work() / "locale" / "simbuffs.js", "w"), nbuffs, json::mJSCall, "_L.patch.add");
}

json::Value makeCat() {
  Dictionary str = readStrings("SkillsUI", &euLoader());
  Dictionary strEu = readStrings("SkillsUI", &euLoader());
  struct { char const* from; char const* to; } classes[] = {
    { "Cat_Wiz", "wizard" },
    { "Cat_DHunter", "demonhunter" },
    { "Cat_WitchDoc", "witchdoctor" },
    { "Cat_Monk", "monk" },
    { "Cat_Barb", "barbarian" },
    { "Cat_Crusader", "crusader" },
  };
  json::Value cats;
  for (int cls = 0; cls < 6; ++cls) {
    json::Value& cat = cats[classes[cls].to];
    for (int idx = 0; idx < 6; ++idx) {
      std::string id = fmtstring("%s%d", classes[cls].from, idx + 1);
      std::string catEu = strlower(strEu[id]);
      std::string catCn = str[id];
      cat[catEu] = catCn;
    }
  }
  return cats;
}
void fixCat() {
  json::Value skills;
  std::string func;
  json::parse(File(path::work() / "locale" / "skills.js"), skills, json::mJSCall, &func);
  skills["skillcat"] = makeCat();
  json::write(File(path::work() / "locale" / "skills.js", "w"), skills, json::mJSCall, func.c_str());
}

std::string strnorm(std::string const& src) {
  static re::Prog spaces("\\s");
  return spaces.replace(strlower(src), "");
}
std::string translate2(std::string const& orig, std::string const& from, std::string const& to) {
  static re::Prog untag("<[^>]+>");
  static re::Prog orig_parser("\\+?%(d|(\\.[0-9])?f)|\\+?[0-9]+(\\.[0-9]+)?");
  static re::Prog fmt_parser("\\+?\\{[^}]+\\}|\\+?\\[[^\\]]+\\]|\\+?[0-9]+{\\.[0-9]+}?|\\n+|\\|[0-9][^:]*:([^;]*);");
  static re::Prog fmt_color("\\{c_([a-z]+)\\}");
  static re::Prog fmt_color_end("\\{/c{_[a-z]+}?\\}");
  std::string tmp = untag.replace(orig, "");
  std::vector<std::string> opts = orig_parser.findAll(tmp);
  uint32 index = 0;
  Dictionary ref;
  fmt_parser.findAll(from, [&opts, &index, &ref](re::Match const& match) {
    std::string whole = match.group(0);
    if (whole[0] == '+') whole.erase(0, 1);
    if (whole[0] == '\n') {
      return;
    } else if (whole[0] == '{') {
      if (whole == "{icon:bullet}") return;
      if (whole[1] == 'c') return;
      if (whole[1] == '/') return;
      if (index >= opts.size()) return;
      ref[strnorm(whole)] = opts[index++];
    } else if (whole[0] == '[') {
      if (index >= opts.size()) return;
      ref[strnorm(whole)] = opts[index++];
    } else if (whole[0] == '|') {
      return;
    } else {
      index++;
    }
  });
  return fmt_parser.replace(to, [&ref](re::Match const& match) {
    std::string whole = match.group(0);
    if (whole[0] == '+') whole.erase(0, 1);
    if (whole[0] == '\n') {
      return std::string("</p><p>");
    } else if (whole[0] == '{') {
      std::vector<std::string> sub;
      if (fmt_color.match(whole, &sub)) {
        return fmtstring("<span class=\"d3-color-%s\">", sub[1].c_str());
      }
      if (fmt_color_end.match(whole)) {
        return std::string("</span>");
      }
      if (whole == "{icon:bullet}") return std::string();
      if (whole[1] == '/') return std::string();
      return ref[strnorm(whole)];
    } else if (whole[0] == '[') {
      return ref[strnorm(whole)];
    } else if (whole[0] == '|') {
      return match.group(1);
    } else {
      return match.group(0);
    }
  });
}

int getUnlocked(std::string const& desc) {
  static std::string unlocked = "Unlocked at level <em>";
  size_t pos = desc.find(unlocked);
  if (pos == std::string::npos) return 0;
  pos += unlocked.length();
  return atoi(desc.c_str() + pos);
}

void fixSkillTips(json::Value& skills) {
  Dictionary name_cn = readStrings("Powers");
  Dictionary name_eu = readStrings("Powers", &euLoader());
  Dictionary attr_cn = readStrings("AttributeDescriptions");
  Dictionary attr_eu = readStrings("AttributeDescriptions", &euLoader());
  Dictionary name2pow;

  std::string desc_begin = "<div class=\"description\"> <p>";
  std::string rune_begin = "<p>";
  std::string desc_end = "</p> <p class=";

  for (auto& kit : SnoLoader::All<SkillKit>()) {
    if (!kit->x10_TraitEntries.size()) continue;
    for (auto& trait : kit->x10_TraitEntries) {
      std::string power = trait.x00_PowerSno.name();
      if (name_eu.count(power + "_name")) {
        name2pow[name_eu[power + "_name"]] = power;
      }
    }
    for (auto& skill : kit->x20_ActiveSkillEntries) {
      std::string power = skill.x00_PowerSno.name();
      if (name_eu.count(power + "_name")) {
        name2pow[name_eu[power + "_name"]] = power;
      }
    }
  }

  json::Value skills_eu, skills_cn;
  json::parse(File(path::work() / "skills.js"), skills_eu, json::mJSCall);
  json::parse(File(path::work() / "skills_cn.js"), skills_cn, json::mJSCall);

  std::string suffix = skills_cn["tipsuffix"].getString();
  std::string rune_suffix = skills_cn["runesuffix"].getString();
  std::string passive_suffix = skills_cn["passivesuffix"].getString();

  json::Value& skilltips = skills["skilltips"];
  json::Value& passivetips = skills["passivetips"];

  for (auto& clsit = skilltips.begin(); clsit != skilltips.end(); ++clsit) {
    for (auto& it = clsit->begin(); it != clsit->end(); ++it) {
      auto& skill = skills_eu["skills"][clsit.key()][it.key()];
      std::string name = skill["name"].getString();
      if (name2pow.count(name)) {
        std::string power = name2pow[name];
        std::string desc = (*it)["x"].getString();
        std::string desc_eu = name_eu[power + "_desc"];
        std::string desc_cn = name_cn[power + "_desc"];

        if (name == "Vengeance") {
          int asdf = 0;
        }

        size_t first = desc.find(desc_begin);
        if (first == std::string::npos) {
          continue;
        }
        first += desc_begin.length();
        size_t last = desc.find(desc_end, first);
        if (last == std::string::npos) {
          continue;
        }

        std::string cat = skill["category"].getString();
        cat = skills_cn["skillcat"][clsit.key()][cat].getString();
        int level = getUnlocked(desc);

        std::string newdesc = desc.substr(0, first);
        newdesc += translate2(desc.substr(first, last - first), desc_eu, desc_cn);
        newdesc += fmtstring(suffix.c_str(), cat.c_str(), level);
        (*it)["x"] = newdesc;

        for (int runei = 0; runei < 5; ++runei) {
          std::string runec = fmtstring("%c", 'a' + runei);
          std::string runeattr = fmtstring("Rune_%c#%s", 'A' + runei, power.c_str());
          std::string desc = (*it)[runec].getString();
          std::string desc_eu = attr_eu[runeattr];
          std::string desc_cn = attr_cn[runeattr];

          size_t first = desc.find(rune_begin);
          if (first == std::string::npos) {
            continue;
          }
          first += rune_begin.length();
          size_t last = desc.find(desc_end, first);
          if (last == std::string::npos) {
            continue;
          }

          int level = getUnlocked(desc);

          std::string newdesc = desc.substr(0, first);
          newdesc += translate2(desc.substr(first, last - first), desc_eu, desc_cn);
          newdesc += fmtstring(rune_suffix.c_str(), level);
          (*it)[runec] = newdesc;
        }
      }
    }
  }

  re::Prog quot("\"");

  for (auto& clsit = passivetips.begin(); clsit != passivetips.end(); ++clsit) {
    for (auto& it = clsit->begin(); it != clsit->end(); ++it) {
      auto& skill = skills_eu["passives"][clsit.key()][it.key()];
      std::string name = skill["name"].getString();
      if (name2pow.count(name)) {
        std::string power = name2pow[name];
        std::string desc = it->getString();
        std::string desc_eu = name_eu[power + "_var_stats"];
        std::string desc_cn = name_cn[power + "_var_stats"];
        std::string flavor_cn = name_cn[power + "_desc"];
        flavor_cn = quot.replace(flavor_cn, "&quot;");

        size_t first = desc.find(desc_begin);
        if (first == std::string::npos) {
          continue;
        }
        first += desc_begin.length();
        size_t last = desc.find(desc_end, first);
        if (last == std::string::npos) {
          continue;
        }

        int level = getUnlocked(desc);

        std::string newdesc = desc.substr(0, first);
        newdesc += translate2(desc.substr(first, last - first), desc_eu, desc_cn);
        newdesc += fmtstring(passive_suffix.c_str(), level, flavor_cn.c_str());
        *it = newdesc;
      }
    }
  }
}
void fixSkillTips() {
  json::Value skills;
  std::string func;
  json::parse(File(path::work() / "locale" / "skilltips2.js"), skills, json::mJSCall, &func, true);
  fixSkillTips(skills);
  json::write(File(path::work() / "locale" / "skilltips.js", "w"), skills, json::mJSCall, func.c_str());
}

std::string getstr(File& file, uint32 offs) {
  file.seek(offs - 0x801000);
  std::string res;
  while (char c = file.getc()) {
    res.push_back(c);
  }
  return res;
}

void save_dict(std::string const& name, Dictionary const& dict) {
  json::Value val;
  for (auto& kv : dict) {
    val[kv.first] = kv.second;
  }
  json::write(File(name, "w"), val);
}
Dictionary load_dict(std::string const& name) {
  Dictionary dict;
  json::Value val;
  json::parse(File(name), val);
  for (auto& kv : val.getMap()) {
    dict[kv.first] = kv.second.getString();
  }
  return dict;
}

void compact(std::string const& name, int mode = json::mJSON) {
  json::Value value;
  json::parse(File(name), value, mode);
  File dst(name, "w");
  dst.printf("var Powers=");
  json::WriterVisitor writer(dst, mode);
  value.walk(&writer);
  writer.onEnd();
  dst.printf(";");
}

void file_copy(File& file, std::string const& from) {
  static char buf[65536];
  File src(from, "rt");
  while (uint32 count = src.read(buf, sizeof buf)) {
    file.write(buf, count);
  }
}

void make_diff(std::string const& name, std::vector<std::string> const& fields, bool links = false) {
  json::Value live, ptr;
  json::parse(File(name + "_live.js"), live);
  json::parse(File(name + "_ptr.js"), ptr);
  File out("diff/" + name + ".html", "w");
  file_copy(out, "diff/templates/" + name + ".html");
  if (links) file_copy(out, "menu.html");
  diff(out, live, ptr, fields, links);
  file_copy(out, "diff/templates/tail.html");
}
void make_html(std::string const& name, bool ptr, std::vector<std::string> const& fields, bool links = false) {
  json::Value value;
  json::parse(File(name + (ptr ? "_ptr.js" : "_live.js")), value);
  File out("game/" + name + (ptr ? "_ptr.html" : "_live.html"), "w");
  file_copy(out, "game/templates/" + name + ".html");
  if (ptr) {
    out.printf(" | <a class=\"live_link\" href=\"/game/%s\">Live</a>", name.c_str());
  } else {
    out.printf(" | <a class=\"ptr_link\" href=\"/game/%s\">PTR</a>", name.c_str());
  }
  out.printf(" | <a class=\"ptr_link\" href=\"/diff/%s\">Diff</a>\n", name.c_str());
  makehtml(out, value, fields, links);
  file_copy(out, "game/templates/tail.html");
}
void make_html(std::string const& name, std::vector<std::string> const& fields, bool links = false) {
  make_html(name, false, fields, links);
  make_html(name, true, fields, links);
}

void dumpTags() {
  ExeFile exe("G:\\D3Ptr\\Diablo III.exe");
  json::Value tags;
  for (uint32 offs = 0x01128E10; offs < 0x011353BC; offs += 44) {
    if (offs == 0x011305DC) {
      offs += 4;
    }
    exe.seek(offs);
    uint32 id = exe.read32();
    exe.seek(12, SEEK_CUR);
    uint32 a = exe.read32();
    uint32 b = exe.read32();
    auto& tag = tags[fmtstring("%d", id)];
    tag["name"] = exe.readString(a);
    tag["tag"] = exe.readString(b);
  }
  json::write(File("tags.txt", "w"), tags);
}

void make_diffs() {
  //make_diff("items", { "name", "set", "icon", "flavor", "primary", "secondary", "powers" });
  //make_diff("itemsets", { "name", "2", "3", "4", "5", "6", "powers" });
  //make_diff("powers", { "name", "desc", "flavor", "rune_a", "rune_b", "rune_c", "rune_d", "rune_e" }, true);
  make_html("items", { "name", "set", "icon", "flavor", "primary", "secondary", "powers" });
  make_html("itemsets", { "name", "2", "3", "4", "5", "6", "powers" });
  //  make_html("powers", {"name", "desc", "flavor", "rune_a", "rune_b", "rune_c", "rune_d", "rune_e"}, true);
}

void make_menu() {
  struct {
    std::string fn, name;
  } classes[] = {
    { "Barbarian", "Barbarian" },
    { "Demonhunter", "Demon Hunter" },
    { "Monk", "Monk" },
    { "WitchDoctor", "Witch Doctor" },
    { "Wizard", "Wizard" },
    { "X1_Crusader", "Crusader" },
    { "Templar", "Templar" },
    { "Scoundrel", "Scoundrel" },
    { "Enchantress", "Enchantress" },
  };

  File mf("menu.html", "w");
  json::Value pows_ptr, pows_live;
  json::parse(File("powers_ptr.js"), pows_ptr);
  json::parse(File("powers_live.js"), pows_live);
  jsonCompare(pows_ptr, pows_live);
  std::set<istring> plist;
  for (auto& kv : pows_ptr.getMap()) plist.insert(kv.first);
  for (auto& kv : pows_live.getMap()) plist.insert(kv.first);
  Dictionary pown = readStrings("Powers");
  mf.printf("  <div class=\"menu\">\n");
  for (auto& cls : classes) {
    SnoFile<SkillKit> kit(cls.fn);
    std::vector<std::string> skills, traits;
    for (auto& skill : kit->x20_ActiveSkillEntries) {
      std::string power = skill.x00_PowerSno.name();
      if (plist.find(power) != plist.end()) {
        skills.push_back(fmtstring("   <a href=\"#%s\">%s</a>\n", power.c_str(), pown[power + "_name"].c_str()));
      }
    }
    for (auto& trait : kit->x10_TraitEntries) {
      std::string power = trait.x00_PowerSno.name();
      if (plist.find(power) != plist.end()) {
        traits.push_back(fmtstring("   <a href=\"#%s\">%s</a>\n", power.c_str(), pown[power + "_name"].c_str()));
      }
    }
    if (!skills.empty() || !traits.empty()) {
      mf.printf("   <h4>%s</h4>\n", cls.name.c_str());
      for (auto& str : skills) mf.printf("%s", str);
      if (!skills.empty() && !traits.empty()) mf.printf("   <br/>\n");
      for (auto& str : traits) mf.printf("%s", str);
    }
  }
  mf.printf("  </div>");
  //json::Value powers;
  //for (auto& pow : casc.json<Power>()) {
  //  powers[Power::name(pow["x000_Header"]["id"].getInteger())] = dumpPower(pow);
  //}
  //json::write(File("powers" + suffix, "w"), powers, json::mJS);
  //return 0;
}

void dump_data(std::string const& suffix) {
  json::Value js_sets;
  json::Value js_items;
  json::Value js_powers;
  for (auto& gmb : SnoLoader::All<GameBalance>()) {
    for (auto& val : gmb->x168_SetItemBonusTable) {
      parseSetBonus(val, js_sets, false);
    }
    for (auto& val : gmb->x028_Items) {
      parseItem(val, js_items, false);
    }
  }
  for (auto& pow : SnoLoader::All<Power>()) {
    //if (pow->x000_Header.id != 79076) continue;
    parsePower(*pow, js_powers, false);
  }
  json::write(File("itemsets" + suffix, "w"), js_sets);
  json::write(File("items" + suffix, "w"), js_items);
  json::write(File("powers" + suffix, "w"), js_powers);
}

void copy(File& src, File& dst) {
  static uint8 buf[65536];
  while (uint32 count = src.read(buf, sizeof buf)) {
    dst.write(buf, count);
  }
}

void write_types() {
  SnoFile<GameBalance> gmb("ItemTypes");
  json::Value types(json::Value::tObject);
  std::map<uint32, json::Value*> dir;
  for (auto& type : gmb->x018_ItemTypes) {
    uint32 id = HashNameLower(type.x000_Text);
    auto it = dir.find(type.x108_ItemTypesGameBalanceId);
    json::Value* parent = &types;
    if (it != dir.end()) {
      parent = it->second;
    }
    auto& val = (*parent)[type.x000_Text];
    val.setType(json::Value::tObject);
    dir[id] = &val;
  }
  json::write(File("ItemTypes.txt", "w"), types);
}

void jsex(std::string const& path) {
  json::Value value;
  std::string func;
  json::parse(File(path), value, json::mJSCall, &func, true);
  json::write(File(path, "w"), value, json::mJSCall, func.c_str());
}

#include "types/Actor.h"
#include "textures.h"
std::map<std::string, uint32> get_item_icons(bool leg = false) {
  int order[] = { 1, 0, 2, 3, 5, 4, 6, 7, 8, 9, 10, 11 };
  std::map<uint32, uint32> icons;
  for (auto& actor : SnoLoader::All<Actor>()) {
    for (int i : order) {
      uint32 id = (&actor->x310_InventoryImages[i / 2].x00)[i % 2];
      if (id == 0 || id == -1) continue;
      icons[actor->x000_Header.id] = id;
      break;
    }
  }
  std::map<std::string, uint32> result;
  for (auto& gmb : SnoLoader::All<GameBalance>()) {
    for (auto& item : gmb->x028_Items) {
      if (!item.x110_Bit0 && leg) continue;
      result[item.x000_Text] = icons[item.x108_ActorSno];
    }
  }
  return result;
}
void item_icons(std::set<std::string> const& filter = {}) {
  auto icons = get_item_icons(true);
  for (auto& kv : icons) {
    if (filter.size() && filter.count(kv.first) == 0) continue;
    Image icon = GameTextures::get(kv.second);
    if (icon) {
      icon.write(fmtstring("items/%s.png", kv.first.c_str()));
    }
  }
}
void item_icons_diff() {
  json::Value lhs, rhs;
  json::parse(File("items_live.js"), lhs);
  json::parse(File("items_ptr.js"), rhs);
  std::set<std::string> diff;
  for (auto& id : rhs.getMap()) {
    if (!lhs.has(id.first)) {
      diff.insert(id.first);
    }
  }
  item_icons(diff);
}

struct IconItemType {
  std::vector<uint32> icons;
  int left = -1, top, right, bottom;
  std::map<uint32, int> added;
  int add(uint32 id) {
    if (added.count(id)) return added[id];
    int index = icons.size();
    icons.push_back(id);
    return added[id] = index;
  }
  Image image(int final_size = 24) {
    if (icons.empty()) return Image();
    int size = std::max(right - left, bottom - top);
    Image result(size, size * icons.size());
    for (size_t i = 0; i < icons.size(); ++i) {
      Image img = GameTextures::get(icons[i]);
      if (!img) continue;
      img = img.subimage(left, top, right, bottom);
      result.blt((size - img.width()) / 2, size * i + (size - img.height()) / 2, img);
    }
    return result.resize(final_size, final_size * icons.size());
  }
};
void item_flavor(SnoLoader* loader = SnoLoader::default) {
  json::Value src, dst, iconsrc, icondst, extra, dyes;
  json::parse(File("itemtypes.js"), src, json::mJS);
  json::parse(File("webgl_items.js"), extra, json::mJS);
  json::parse(File("typeicons.js"), iconsrc);
  json::parse(File("dyes.js"), dyes, json::mJS);
  DictionaryRef flavor = Strings::list("ItemFlavor", loader);
  std::map<std::string, IconItemType> types;

  auto icons = get_item_icons();
  for (auto& kv : iconsrc["sizes"].getMap()) {
    auto& t = types[kv.first];
    t.left = kv.second[0].getInteger();
    t.top = kv.second[1].getInteger();
    t.right = t.left + kv.second[2].getInteger();
    t.bottom = t.top + kv.second[3].getInteger();
  }
  for (auto& kv : src["itemById"].getMap()) {
    if (flavor.has(kv.first)) {
      dst["itemById"][kv.first]["flavor"] = flavor[kv.first];
    }
    if (icons.count(kv.first)) {
      auto& type = types[kv.second["type"].getString()];
      if (type.left == -1) {
        throw Exception("unknown type %s", kv.second["type"].getString().c_str());
      }
      icondst[kv.first] = type.add(icons[kv.first]);
    } else {
      Logger::log("item not found: %s", kv.first.c_str());
    }
  }
  for (auto& kv : extra.getMap()) {
    if (!kv.second.has("type")) continue;
    if (icons.count(kv.first)) {
      auto& type = types[kv.second["type"].getString()];
      if (type.left == -1) {
        throw Exception("unknown type %s", kv.second["type"].getString().c_str());
      }
      icondst[kv.first] = type.add(icons[kv.first]);
    } else {
      Logger::log("item not found: %s", kv.first.c_str());
    }
  }
  for (auto& kv : dyes.getMap()) {
    if (icons.count(kv.first)) {
      auto& type = types["dyes"];
      if (type.left == -1) {
        throw Exception("unknown type dyes");
      }
      icondst[kv.first] = type.add(icons[kv.first]);
    } else {
      Logger::log("item not found: %s", kv.first.c_str());
    }
  }
  //for (auto& kv : src["itemTypes"].getMap()) {
  //  std::string id = kv.second["generic"].getString();
  //  if (icons.count(id)) {
  //    auto& type = types[kv.first];
  //    if (type.left == -1) {
  //      throw Exception("unknown type %s", kv.first.c_str());
  //    }
  //    icondst[id] = type.add(icons[id]);
  //  } else {
  //    Logger::log("item not found: %s", id.c_str());
  //  }
  //}
  for (auto& kv : src["legendaryGems"].getMap()) {
    std::string id = kv.second["id"].getString();
    if (icons.count(id)) {
      auto& type = types["gemleg"];
      if (type.left == -1) {
        throw Exception("unknown type gemleg");
      }
      icondst[id] = type.add(icons[id]);
    } else {
      Logger::log("item not found: %s", id.c_str());
    }
  }
  for (auto& type : types) {
    Image img = type.second.image();
    if (img) {
      img.write("itemtypes/" + type.first + ".png");
    }
  }
  json::write(File("itemflavor.js", "w"), dst, json::mJSCall, "_L.patch.add");
  File df("item_icons.js", "w");
  df.printf("DiabloCalc.itemIcons = ");
  json::write(df, icondst, json::mJS);
}
void translate_extra() {
  json::Value dst, extra, dyesrc, dyedst;
  json::parse(File("webgl_items.js"), extra, json::mJS);
  json::parse(File("dyes.js"), dyesrc, json::mJS);
  DictionaryRef names = Strings::list("Items", &cnLoader());
  for (auto& kv : extra.getMap()) {
    if (!kv.second.has("name")) continue;
    dst[kv.first]["name"] = names[kv.first];
  }
  for (auto& kv : dyesrc.getMap()) {
    if (!kv.second.has("name")) continue;
    dyedst[kv.first]["name"] = names[kv.first];
  }
  json::Value out;
  out["webglItems"] = dst;
  out["webglDyes"] = dyedst;
  json::write(File("extra_names.js", "w"), out, json::mJSCall, "_L.patch.add");
}
Image image_resize(Image src, int width, int height) {
  int nw = src.width() * height / src.height();
  int nh = src.height() * width / src.width();
  if (nw <= width) {
    src = src.resize(nw, height, ImageFilter::CubicConvolution);
  } else {
    src = src.resize(width, nh, ImageFilter::CubicConvolution);
  }
  Image res(width, height);
  res.blt((width - src.width()) / 2, (height - src.height()) / 2, src);
  return res;
}
Image image_fit(Image src, int width, int height) {
  Image res(width, height);
  res.blt((width - src.width()) / 2, (height - src.height()) / 2, src);
  return res;
}
void fix_chests() {
  json::Value src;
  json::parse(File("itemlist.js"), src, json::mJS);
  auto icons = get_item_icons();
  for (auto& kv : src["itemById"].getMap()) {
    if (kv.second["type"].getString() == "chestarmor" || kv.second["type"].getString() == "cloak") {
      Image icon = GameTextures::get(icons[kv.first]);
      if (icon && (icon.width() != 82 || icon.height() != 164)) {
        image_fit(icon, 82, 164).write("chestarmor/" + kv.first + ".png");
        //image_resize(icon, 82, 164).write("chestarmor/" + kv.first + ".png");
      }
    }
  }
}

void setorder() {
  json::Value dst;
  auto stl = Strings::list("ItemSets");
  for (auto& gmb : SnoLoader::All<GameBalance>()) {
    for (auto& item : gmb->x028_Items) {
      uint32 setbonus = item.x170_SetItemBonusesGameBalanceId;
      char const* setname = SnoManager::gameBalance()[setbonus];
      if (setname) {
        static re::Prog getname(R"((.*) \([0-9]+\))");
        std::string name = getname.replace(setname, "\\1");
        if (stl.has(name)) {
          dst[stl[name]].append(item.x10C_ItemTypesGameBalanceId.name());
        }
      }
    }
  }
  json::write(File("setorder.js", "w"), dst);
}

void dirlower(std::string const& path) {
  WIN32_FIND_DATA data;
  HANDLE hFind = FindFirstFile((path / "*").c_str(), &data);
  if (hFind == INVALID_HANDLE_VALUE) return;
  std::vector<std::string> list;
  do {
    list.push_back(data.cFileName);
  } while (FindNextFile(hFind, &data));
  FindClose(hFind);
  for (auto& name : list) {
    MoveFile((path / name).c_str(), (path / strlower(name)).c_str());
  }
}

void archon() {
  Image src("class-witchdoctor.png");
  Image dst(src.width(), src.height() + 84);
  dst.blt(0, 0, src, 0, 0, src.width(), src.height());
  std::string powers[] = {
    //"Wizard_Archon_ArcaneStrike",
    //"Wizard_Archon_DisintegrationWave",
    //"Wizard_Archon_ArcaneBlast",
    //"Wizard_Archon_Teleport",
    //"Wizard_Archon_SlowTime",
    "Witchdoctor_Hex_Explode",
  };
  int index = 0;
  json::Value out;
  auto names = Strings::list("Powers");
  for (auto& power : powers) {
    PowerTag* tag = PowerTags::get(power);
    dst.blt(index * 42, dst.height() - 84, GameTextures::get(tag->getint("Icon Normal")).resize(42, 42));
    dst.blt(index * 42, dst.height() - 42, GameTextures::get(tag->getint("Icon Inactive")).resize(42, 42));
    size_t pos = power.find('_');
    auto& val = out[strlower(power.substr(pos + 1))];
    val["name"] = names[power + "_name"];
    val["tip"] = SkillTips::format(power);
    val["row"] = dst.height() / 84 - 1;
    val["col"] = index;
    ++index;
  }
  dst.write("class-witchdoctor.png");
  json::write(File("extra.js", "w"), out, json::mJS);
}

void progress(std::string const& suffix) {
  json::Value out;
  auto names = Strings::list("Monsters");
  for (auto& mon : SnoLoader::All<Monster>()) {
    if (names.has(mon.name()) && mon->x044_float[58]) {
      out[names[mon.name()]] = mon->x044_float[58] * 0.25f;
    }
  }
  json::write(File("monsters" + suffix, "w"), out);
}
struct MonValue {
  istring name;
  double lhs, rhs;
  double delta;
  MonValue() {}
  MonValue(istring const& name, double lhs, double rhs)
    : name(name), lhs(lhs), rhs(rhs)
  {
    delta = rhs / lhs - 1;
  }
};
bool operator < (MonValue const& lhs, MonValue const& rhs) {
  return lhs.delta > rhs.delta;
}
void progress_comp(bool trans) {
  json::Value lhs, rhs;
  json::parse(File("monsters_second.js"), lhs);
  json::parse(File("monsters_latest.js"), rhs);
  std::map<istring, std::pair<double, double>> values;
  for (auto& kv : lhs.getMap()) {
    values[kv.first].first = kv.second.getNumber();
  }
  for (auto& kv : rhs.getMap()) {
    values[kv.first].second = kv.second.getNumber();
  }
  std::vector<MonValue> monsters;
  Dictionary transl;
  if (trans) {
    auto src = readStrings("Monsters");
    auto dst = readStrings("Monsters", &cnLoader());
    for (auto& kv : src) {
      if (dst.count(kv.first)) {
        transl[kv.second] = dst[kv.first];
      }
    }
  }
  for (auto& kv : values) {
    auto name = kv.first;
    if (trans && transl.count(name)) name = transl[name];
    if (!kv.second.first || !kv.second.second) continue;
    monsters.emplace_back(name, kv.second.first, kv.second.second);
  }
  std::sort(monsters.begin(), monsters.end());
  File dst("monsters_diff.csv", "w");
  for (auto& mon : monsters) {
    if (mon.delta) dst.printf("%s,%lf,%lf,%lf\n", mon.name.c_str(), mon.lhs, mon.rhs, mon.delta*100);
  }
}

void json_map(std::string const& dir, bool mag) {
  WIN32_FIND_DATA data;
  HANDLE hFind = FindFirstFile((dir / "*").c_str(), &data);
  if (hFind == INVALID_HANDLE_VALUE) return;
  do {
    std::string path = dir / data.cFileName;
    if (!(GetFileAttributes(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY)) {
      std::cout << data.cFileName << std::endl;
      json::Value src;
      std::string func;
      json::parse(File(path), src, json::mJSCall, &func, true);
      File out(path, "w");
      json::WriterVisitor writer(out, json::mJSCall, func.c_str());
      if (mag) writer.setIndent(2);
      src.walk(&writer);
      writer.onEnd();
    }
  } while (FindNextFile(hFind, &data));
  FindClose(hFind);
}

void dump_affixes() {
  SnoFile<GameBalance> gmb("1xx_AffixList");
  json::Value value;
  for (auto& affix : gmb->x078_AffixTable) {
    for (auto& attr : affix.x260_AttributeSpecifiers) {
      value[fmtstring("%d", attr.x00_Type)] = affix.x000_Text;
    }
  }
  json::write(File("affixlist.js", "w"), value);
}

void dump_animset(File& file, std::string const& name) {
  std::map<uint32, uint32> result;
  SnoFile<AnimSet> animSet(name);
  for (auto& tm : animSet->x010_AnimSetTagMaps) {
    for (uint32 i = 0; i < tm.x08_TagMap[0]; ++i) {
      uint32 type = tm.x08_TagMap[i * 3 + 2];
      uint32 id = tm.x08_TagMap[i * 3 + 3];
      if (Anim::name(id) && !result.count(type)) result[type] = id;
    }
  }
  for (auto& kv : result) {
    file.printf("%-8d %s\n", kv.first, Anim::name(kv.second));
  }
}
void dump_animsets() {
  SnoFile<GameBalance> gmb("Characters");
  for (auto& hero : gmb->x088_Heros) {
    SnoFile<Actor> actor_m(hero.x108_ActorSno.name());
    dump_animset(File(fmtstring("animset_%s_male.txt", hero.x000_Text), "w"), actor_m->x068_AnimSetSno.name());
    SnoFile<Actor> actor_f(hero.x10C_ActorSno.name());
    dump_animset(File(fmtstring("animset_%s_female.txt", hero.x000_Text), "w"), actor_f->x068_AnimSetSno.name());
  }
}
std::string canonize(std::string const& str);
void write_skill_anim() {
  SnoFile<GameBalance> gmb("Characters");
  auto dictPowers = Strings::list("Powers");
  json::Value output;
  for (auto& hero : gmb->x088_Heros) {
    SnoFile<SkillKit> kit(hero.x120_SkillKitSno.name());
    for (auto& skill : kit->x20_ActiveSkillEntries) {
      std::string power = skill.x00_PowerSno.name();
      PowerTag* tag = PowerTags::get(power);
      std::string name = dictPowers[power + "_name"];
      output[canonize(name)] = tag->getint("Animation Tag");
    }
  }
  json::write(File("skillanim.js", "w"), output, json::mJS);
}

void write_item_icon(json::Value& dst, Archive& dsticons, std::string const& id, bool torso) {
  auto* item = ItemLibrary::get(id);
  if (!item) {
    Logger::log("item not found: %s", id.c_str());
    return;
  }
  SnoFile<Actor> actor(item->x108_ActorSno.name());
  if (!actor) {
    Logger::log("actor not found for %s", id.c_str());
    return;
  }
  auto& images = actor->x310_InventoryImages;
  std::set<uint32> icons;
  for (auto& inv : images) {
    if (inv.x00 != 0 && inv.x00 != -1) icons.insert(inv.x00);
    if (inv.x04 != 0 && inv.x04 != -1) icons.insert(inv.x04);
  }
  for (uint32 id : icons) {
    Image image = GameTextures::get(id);
    if (image) {
      if (torso && (image.width() != 82 || image.height() != 164)) {
        Image tmp(82, 164);
        tmp.blt(41 - image.width() / 2, 82 - image.height() / 2, image);
        image = tmp;
      }
      image.write(dsticons.create(id), ImageFormat::PNG);
    }
  }
  auto& data = dst[id];
  if (data.type() != json::Value::tArray) {
    data.append(0);
  }
  if (icons.size() == 1) {
    data.append(*icons.begin());
  } else if (!icons.empty()) {
    auto& value = data.append(json::Value::tObject);
    for (size_t i = 0; i < 6; ++i) {
      auto& inv = images[i];
      if (inv.x00 != 0 && inv.x00 != -1) value[fmtstring("%d", i * 2 + 0)] = (uint32)inv.x00;
      if (inv.x04 != 0 && inv.x04 != -1) value[fmtstring("%d", i * 2 + 1)] = (uint32)inv.x04;
    }
  }
}

void write_item_icons() {
  json::Value src, srcextra, dst;
  json::parse(File("itemtypes.js"), src, json::mJS);
  json::parse(File("webgl_items.js"), srcextra, json::mJS);
  json::parse(File("item_icons.js"), dst, json::mJS);
  for (auto& val : dst) {
    if (val.type() != json::Value::tArray) {
      json::Value tmp = val;
      val.append(tmp);
    } else {
      json::Value tmp = (val.length() < 1 ? 0 : val[0]);
      val.clear();
      val.append(tmp);
    }
  }
  Archive dsticons;
  dsticons.load(File("item_icons.wgz"), false);
  //for (auto& kv : src["itemById"].getMap()) {
  //  std::string id = kv.first;
  //  std::string type = kv.second["type"].getString();
  //  std::string slot = src["itemTypes"][type]["slot"].getString();
  //  write_item_icon(dst, dsticons, id, slot == "torso");
  //}
  //for (auto& kv : src["legendaryGems"].getMap()) {
  //  std::string id = kv.second["id"].getString();
  //  write_item_icon(dst, dsticons, id, false);
  //}
  //uint32 numQualities = src["gemQualities"].length();
  //for (auto& kv : src["gemColors"].getMap()) {
  //  std::string id = kv.second["id"].getString();
  //  for (uint32 level = 1; level <= numQualities; ++level) {
  //    write_item_icon(dst, dsticons, id + fmtstring("%02d", level), false);
  //  }
  //}
  //for (auto& kv : srcextra.getMap()) {
  //  if (!kv.second.has("type")) continue;
  //  std::string id = kv.first;
  //  std::string type = kv.second["type"].getString();
  //  std::string slot = src["itemTypes"][type]["slot"].getString();
  //  write_item_icon(dst, dsticons, id, slot == "torso");
  //}
  write_item_icon(dst, dsticons, "Unique_Offhand_001_x1", false);
  json::write(File("item_icons.js", "w"), dst, json::mJS);
  dsticons.write(File("item_icons.wgz", "wb"), false);
}

void itemPowerFilter() {
  std::string classes[] = {"wizard", "demonhunter", "witchdoctor", "monk", "barbarian", "crusader"};
  std::map<uint32, std::string> classMap;
  for (auto& str : classes) {
    classMap[HashNameLower(str)] = str;
  }
  json::Value items, out;
  json::parse(File("itempowers.js"), items, json::mJS);
  uint32 powerAttr = fixAttrId(1270, true);
  for (auto& it : items["itemById"].getMap()) {
    auto* item = ItemLibrary::get(it.first);
    if (!item) {
      Logger::log("Item not found: %s", it.first.c_str());
      continue;
    }
    std::string restriction;
    for (auto& attr : item->x1F8_AttributeSpecifiers) {
      if (attr.x00_Type == powerAttr) {
        auto* tag = PowerTags::getraw(attr.x04_Param);
        if (!tag) {
          Logger::log("Power not found: %u (%s)", attr.x04_Param, it.first.c_str());
          continue;
        }
        uint32 cls = tag->getint("Class Restriction");
        if (!classMap.count(cls)) continue;
        restriction = classMap[cls];
      }
    }
    if (!restriction.empty()) {
      out[it.second["required"]["custom"]["id"].getString()] = restriction;
    }
  }
  json::write(File("itemclasses.js", "wt"), out, json::mJS);
}

void fixSets2() {
  json::Value sets, good;
  std::string func;
  json::parse(File("locale/itemsets.js"), sets, json::mJSCall, &func);
  json::parse(File("locale/cn_itemsets.js"), good, json::mJS);
  for (auto it = sets["itemSets"].begin(); it != sets["itemSets"].end(); ++it) {
    if (!good.has(it.key())) continue;
    auto& src = good[it.key()];
    (*it)["name"] = src["name"];
    if (it->has("bonuses")) {
      for (auto bit = (*it)["bonuses"].begin(); bit != (*it)["bonuses"].end(); ++bit) {
        if (!src.has(bit.key())) continue;
        auto& bsrc = src[bit.key()];
        for (size_t i = 0; i < bsrc.length(); ++i) {
          std::string key = fmtstring("%d", i);
          if (!bit->has(key)) continue;
          std::string fmt;
          for (char c : bsrc[i].getString()) {
            if (c == '%') fmt.push_back(c);
            fmt.push_back(c);
          }
          (*bit)[key]["format"] = fmt;
        }
      }
    }
  }
  json::write(File("locale/itemsets.js", "w"), sets, json::mJSCall, func.c_str());
}
