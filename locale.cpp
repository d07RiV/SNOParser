#include "common.h"
#include "description.h"
#include "miner.h"
#include "strings.h"
#include "powertag.h"
#include "affixes.h"
#include "translations.h"
#include "itemlib.h"
#include "types/Recipe.h"

template<class Func>
bool testString(std::string const& str, Func const& func) {
  for (char c : str) {
    if (!func((uint8) c)) return false;
  }
  return true;
}

std::string FormatTag(std::string const& src, json::Value& data) {
  if (src.empty()) return src;
  if (src[0] == '$' && src.back() == '$') return src;
  size_t slash = src.find('/');
  if (slash == std::string::npos) return src;
  auto list = Strings::list(src.substr(0, slash));
  if (!list) {
    Logger::log("StringList not found: %s", src.substr(0, slash).c_str());
    return src;
  }
  size_t at = src.find('@', slash);
  std::string text = src.substr(slash + 1, at == std::string::npos ? at : at - slash - 1);
  if (!list.has(text)) {
    Logger::log("String not found: %s", src.substr(0, at).c_str());
    return src;
  }
  text = list[text];
  if (at != std::string::npos) {
    size_t quest = src.find('?', at);
    if (quest == std::string::npos) quest = src.length();
    FormatFlags flags = FormatTags;
    PowerTag* context = nullptr;
    AttributeMap values = GameAffixes::defaultMap();
    if (quest > at + 1) {
      flags = FormatHTML;
      context = PowerTags::get(src.substr(at + 1, quest - at - 1));
      if (!context) {
        Logger::log("PowerTag not found: %s", src.substr(at + 1, quest - at - 1).c_str());
      }
    } else {
      values["value"] = AttributeValue("%");
      values["value1"] = AttributeValue("%");
      values["value2"] = AttributeValue("%");
      values["value3"] = AttributeValue("%");
    }
    std::map<std::string, std::string> replace;
    if (quest < src.length()) {
      std::vector<std::string> parts = split(src.substr(quest + 1), '&');
      for (auto& p : parts) {
        size_t eq = p.find('=');
        if (eq == std::string::npos) continue;
        std::string key = p.substr(0, eq);
        std::string value = p.substr(eq + 1);
        if (!testString(key, isalnum)) {
          size_t pos = text.find(key);
          if (pos == std::string::npos) continue;
          std::string rval = fmtstring("@%d", replace.size() + 1);
          replace.emplace(rval, value);
          text.replace(pos, key.length(), rval);
          continue;
        }
        if (testString(value, isdigit)) {
          values.emplace(key, atof(value.c_str()));
          continue;
        }
        size_t ss = value.find('/');
        if (ss == std::string::npos) {
          values[key] = AttributeValue(value);
        } else {
          std::string listname = value.substr(0, ss);
          if (!testString(listname, isalpha)) {
            values[key] = AttributeValue(value); continue;
          }
          auto sublist = Strings::list(listname);
          if (!sublist) {
            Logger::log("StringList not found: %s", listname.c_str());
            continue;
          }
          std::string subtext = value.substr(ss + 1);
          if (!sublist.has(subtext)) {
            Logger::log("String not found: %s", value.c_str());
            continue;
          }
          values[key] = AttributeValue(sublist[subtext]);
        }
      }
    }
    text = FormatDescription(text, flags, values, context);
    for (auto& kv : replace) {
      size_t pos = text.find(kv.first);
      if (pos == std::string::npos) continue;
      text.replace(pos, kv.first.length(), kv.second);
    }
  }
  return text;
}

typedef void(*JsonModifier)(json::Value& value, json::Value& data);

void NullFunc(json::Value& value, json::Value& data) {
}

std::string StoreTag(std::string const& src, json::Value& data) {
  if (src[0] == '$' && src.back() == '$') {
    size_t pos = src.find('/');
    if (pos != std::string::npos) {
      std::string cat = src.substr(1, pos - 1);
      std::string name = src.substr(pos + 1, src.length() - pos - 2);
      if (!data["stringlist"][cat].has(name)) {
        data["stringlist"][cat][name]["text"] = name;
      }
    }
  }
  return src;
}
std::string FormatAndStoreTag(std::string const& src, json::Value& data) {
  StoreTag(src, data);
  return FormatTag(src, data);
}

template<class Func>
void TransformJson(json::Value& value, json::Value& data, Func const& func) {
  switch (value.type()) {
  case json::Value::tString:
    value = func(value.getString(), data);
    break;
  case json::Value::tArray:
  case json::Value::tObject:
    for (auto& sub : value) {
      TransformJson(sub, data, func);
    }
    break;
  }
}

void TranslateJson(json::Value& value, json::Value& data) {
  TransformJson(value, data, FormatAndStoreTag);
}
void StoreJson(json::Value& value, json::Value& data) {
  TransformJson(value, data, StoreTag);
}

void GenerateExtraItems(json::Value& value, json::Value& data) {
  auto list = Strings::list("Items");
  for (auto& kv : data["webglItems"].getMap()) {
    if (!kv.second.has("name")) continue;
    if (!list.has(kv.first)) {
      Logger::log("Item not found: %s", kv.first.c_str());
    } else {
      value["webglItems"][kv.first]["name"] = untag(list[kv.first], true);
    }
  }
  for (auto& kv : data["webglDyes"].getMap()) {
    if (!list.has(kv.first)) {
      Logger::log("Item not found: %s", kv.first.c_str());
    } else {
      value["webglDyes"][kv.first]["name"] = list[kv.first];
    }
  }
}

void GenerateGems(json::Value& value, json::Value& data) {
  TranslateJson(value, data);
  auto names = Strings::list("Items");
  auto powers = Strings::list("ItemPassivePowerDescriptions");
  for (auto& gem : data["legendaryGems"].getMap()) {
    std::string id = gem.second["id"].getString();
    auto* item = ItemLibrary::get(id);
    if (!names.has(id) || !item) {
      Logger::log("Item not found: %s", id.c_str());
      continue;
    }
    auto& dst = value["legendaryGems"][gem.first];
    dst["name"] = names[id];
    if (gem.second.has("effects")) {
      if (gem.second["effects"].has("0") && gem.second["effects"]["0"].has("format")) {
        char const* name = Power::name(item->x4B8_AttributeSpecifier.x04_Param);
        auto* tag = PowerTags::getraw(item->x4B8_AttributeSpecifier.x04_Param);
        if (!name || !tag || !powers.has(name)) {
          Logger::log("Power(0) not found for %s", id.c_str());
        } else {
          std::string fmt = powers[name];
          auto values = GameAffixes::defaultMap();
          values["value1"] = AttributeValue("%");
          dst["effects"]["0"]["format"] = FormatDescription(powers[name], FormatTags, values, tag);
        }
      }
      if (gem.second["effects"].has("1") && gem.second["effects"]["1"].has("format")) {
        std::vector<AttributeSpecifier> specs;
        specs.emplace_back(item->x4D0_AttributeSpecifier);
        auto format = GameAffixes::format(specs, FormatTags);
        if (format.empty()) {
          Logger::log("Unknown power(1) for %s", id.c_str());
        } else {
          dst["effects"]["1"]["format"] = format[0];
        }
      }
    }
  }
}

void GenerateFlavor(json::Value& value, json::Value& data) {
  auto text = Strings::list("ItemFlavor");
  for (auto& kv : data["itemById"].getMap()) {
    if (text.has(kv.first)) {
      value["itemById"][kv.first]["flavor"] = text[kv.first];
    }
  }
  for (auto& kv : data["legendaryGems"].getMap()) {
    std::string id = kv.second["id"].getString();
    if (text.has(id)) {
      value["legendaryGems"][kv.first]["flavor"] = text[id];
    }
  }
  for (auto& kv : data["webglItems"].getMap()) {
    if (!kv.second.has("name")) continue;
    if (text.has(kv.first)) {
      value["webglItems"][kv.first]["flavor"] = text[kv.first];
    }
  }
  for (auto& kv : data["webglDyes"].getMap()) {
    if (text.has(kv.first)) {
      value["webglDyes"][kv.first]["flavor"] = text[kv.first];
    }
  }
}

void GenerateItems(json::Value& value, json::Value& data) {
  auto names = Strings::list("Items");
  auto powers = Strings::list("ItemPassivePowerDescriptions");
  uint32 powId = fixAttrId(1270, true);
  std::map<std::string, std::string> powerFix;
  powerFix.emplace("Unique_Amulet_109_x1_210", "ItemPassive_x1_Amulet_norm_unique_25_DemonHunter");

  std::map<uint32, GameBalance::Type::AffixTableEntry*> affixes;
  SnoFile<GameBalance> affixFile("1xx_AffixList");
  for (auto& afx : affixFile->x078_AffixTable) {
    affixes.emplace(HashNameLower(afx.x000_Text), &afx);
  }
  for (auto& rcp : SnoLoader::All<Recipe>()) {
    std::string item = rcp->x0C_ItemSpecifierData.x00_ItemsGameBalanceId.name();
    if (item.empty()) continue;
    uint32 power = 0;
    for (auto& id : rcp->x0C_ItemSpecifierData.x08_GameBalanceIds) {
      if (affixes.count(id)) {
        for (auto& attr : affixes[id]->x260_AttributeSpecifiers) {
          if (attr.x00_Type == powId) {
            power = attr.x04_Param;
            break;
          }
        }
      }
      if (power) break;
    }
    char const* pname = (power ? Power::name(power) : nullptr);
    if (pname) powerFix.emplace(item, pname);
  }

  for (auto& kv : data["itemById"].getMap()) {
    auto* item = ItemLibrary::get(kv.first);
    if (!names.has(kv.first) || !item) {
      Logger::log("Item not found: %s", kv.first.c_str());
      continue;
    }
    auto& dst = value["itemById"][kv.first];
    dst["name"] = names[kv.first];
    if (kv.second.has("required")) {
      std::string key = kv.second["required"]["custom"]["id"].getString();
      data["legendaryMap"][key] = kv.first;
      data["stringlist"]["Items"][key]["text"] = kv.second["required"]["custom"]["name"];
      data["stringlist"]["Items"][key]["tip"] = names[kv.first] + " (short effect name)";
      dst["required"]["custom"]["name"] = fmtstring("$Items/%s$", key.c_str());
      bool found = false;
      for (auto& attr : item->x1F8_AttributeSpecifiers) {
        if (attr.x00_Type != powId) continue;
        char const* name = Power::name(attr.x04_Param);
        auto* tag = PowerTags::getraw(attr.x04_Param);
        if (!name || !tag || !powers.has(name)) {
          Logger::log("Power not found for %s", kv.first.c_str());
          continue;
        }
        auto values = GameAffixes::defaultMap();
        AttributeValue value = ExecFormula(
          reinterpret_cast<uint32 const*>(attr.x08_Data.begin()),
          reinterpret_cast<uint32 const*>(attr.x08_Data.end()),
          values);
        if (value.min != value.max) {
          values["value1"] = AttributeValue("%");
        } else {
          values["value1"] = value;
        }
        dst["required"]["custom"]["format"] = FormatDescription(powers[name], FormatTags, values, tag);
        found = true;
        break;
      }
      if (!found) {
        if (powerFix.count(kv.first)) {
          auto* tag = PowerTags::get(powerFix[kv.first]);
          auto values = GameAffixes::defaultMap();
          values["value1"] = AttributeValue("%");
          dst["required"]["custom"]["format"] = FormatDescription(powers[powerFix[kv.first]], FormatTags, values, tag);
        } else if (kv.first == "x1_Amulet_norm_unique_25") {
          // hellfire
          std::string x = powers["ItemPassive_Unique_Ring_770_x1"];
          std::string y = Strings::get("Powers", "Barbarian_Scavenge_name");
          size_t pos = x.find(y);
          if (pos != std::string::npos) {
            x.replace(pos, y.length(), "%p");
            dst["required"]["custom"]["format"] = x;
          }
        } else {
          Logger::log("Power not found for %s", kv.first.c_str());
        }
      }
    }
  }
}

void GenerateItemSets(json::Value& value, json::Value& data) {
  uint32 powId = fixAttrId(1270, true);
  SnoFile<GameBalance> sets("SetItemBonuses");
  std::map<std::string, uint32> setMap;
  std::map<uint32, std::string> setNames;
  std::map<uint32, std::vector<GameBalance::Type::SetItemBonusTableEntry*>> entries;
  auto namesEn = Strings::list("ItemSets", SnoLoader::default);
  auto names = Strings::list("ItemSets");
  for (auto& bonus : sets->x168_SetItemBonusTable) {
    if (bonus.x108_SetItemBonusesGameBalanceId == -1) {
      if (namesEn.has(bonus.x000_Text)) {
        uint32 id = HashNameLower(bonus.x000_Text);
        setMap[namesEn[bonus.x000_Text]] = id;
        setNames[id] = bonus.x000_Text;
      }
    } else if (bonus.x10C) {
      entries[bonus.x108_SetItemBonusesGameBalanceId].push_back(&bonus);
    }
  }

  for (auto& kv : data["itemSets"].getMap()) {
    std::string name = kv.second["name"].getString();
    if (!setMap.count(name)) {
      Logger::log("ItemSet not found: %s", name.c_str());
      continue;
    }
    uint32 id = setMap[name];
    auto& dst = value["itemSets"][kv.first];
    dst["name"] = names[setNames[id]];
    data["setMap"][kv.first] = setNames[id];
    if (!kv.second.has("bonuses")) continue;
    std::map<int, std::vector<std::string>> powers;
    for (auto* bonus : entries[id]) {
      std::vector<AttributeSpecifier> specs;
      for (auto& attr : bonus->x110_AttributeSpecifiers) {
        if (attr.x00_Type == powId) specs.emplace_back(attr);
      }
      if (specs.empty()) continue;
      auto format = GameAffixes::format(specs, FormatTags);
      if (format.empty()) {
        Logger::log("Unknown power for %s", name.c_str());
      } else {
        for (auto& pow : format) {
          powers[bonus->x10C].push_back(pow);
        }
      }
    }
    for (auto& bcount : kv.second["bonuses"].getMap()) {
      int count = atoi(bcount.first.c_str());
      size_t index = 0;
      auto& src = powers[count];
      for (auto& bindex : bcount.second.getMap()) {
        if (bindex.second.has("format") && index < src.size()) {
          dst["bonuses"][bcount.first][bindex.first]["format"] = src[index++];
        }
      }
    }
  }
}

void GenerateSkills(json::Value& value, json::Value& data) {
  TranslateJson(value, data);

  SnoFile<GameBalance> gmb("Characters");
  auto names = Strings::list("Powers");
  auto attrs = Strings::list("AttributeDescriptions");
  for (auto& chr : gmb->x088_Heros) {
    SkillTips tips;
    tips.generate(canonize(chr.x000_Text), chr.x120_SkillKitSno, &data["skillFix"]);
    data["passivetips"][tips.charClass] = tips.passives;
    data["skilltips"][tips.charClass] = tips.skills;
    for (auto& kv : tips.skillMap.getMap()) {
      auto& dst = value["skills"][tips.charClass][kv.first];
      dst["name"] = names[kv.second.getString() + "_name"];
      data["skillMap"][kv.first] = kv.second;
      for (char rune = 'a'; rune <= 'e'; ++rune) {
        //NameRune_A#Witchdoctor_Firebomb
        dst["runes"][std::string{rune}] = attrs.getfmt("NameRune_%c#%s", rune - 'a' + 'A', kv.second.getString().c_str());
      }
    }
    for (auto& kv : tips.passiveMap.getMap()) {
      value["passives"][tips.charClass][kv.first]["name"] = names[kv.second.getString() + "_name"];
      data["skillMap"][kv.first] = kv.second;
    }
    value["skillcat"][tips.charClass] = tips.categoryMap;
  }
  for (auto& cls : value["extraskills"]) {
    for (auto& skill : cls) {
      std::string power = skill["power"].getString();
      skill.remove("power");
      if (!names.has(power + "_name")) {
        Logger::log("Power not found: %s", power.c_str());
        continue;
      }
      skill["name"] = names[power + "_name"];
      skill["tip"] = SkillTips::format(power);
    }
  }
  //data["skillMap"]
}
void GenerateSkillTips(json::Value& value, json::Value& data) {
  value["skilltips"] = data["skilltips"];
  value["passivetips"] = data["passivetips"];
}

void ListUiStats(json::Value& value, json::Value& data) {
  TransformJson(value, data, FormatTag);
  std::string pane = "$StatsPane/";

  for (auto& cat : value["uiStats"]) {
    for (auto& stat : cat["stats"]) {
      if (!stat.has("tooltip") || !stat.has("name")) continue;

      std::string tip = stat["tooltip"].getString();
      if (tip.find(pane) == 0) {
        std::string alt = stat["name"].getString();
        if (alt.find(pane) == 0) {
          alt = alt.substr(pane.length(), alt.length() - pane.length() - 1);
        }
        alt.append(" - Tip");
        data["stringlist"]["StatsPane"][alt]["text"] = tip.substr(pane.length(), tip.length() - pane.length() - 1);
        stat["tooltip"] = fmtstring("$StatsPane/%s$", alt.c_str());
      }
    }
  }

  TransformJson(value, data, StoreTag);
}

void GenerateSimBuffs(json::Value& value, json::Value& data) {
  TransformJson(value, data, FormatTag);

  auto powers = Strings::list("Powers");
  auto attrs = Strings::list("AttributeDescriptions");
  auto powersEn = Strings::list("Powers", SnoLoader::default);
  auto attrsEn = Strings::list("AttributeDescriptions", SnoLoader::default);
  auto sets = Strings::list("ItemSets");
  auto setsEn = Strings::list("ItemSets", SnoLoader::default);
  auto items = Strings::list("Items");
  auto itemsEn = Strings::list("Items", SnoLoader::default);

  for (auto& kv : data["simMapping"]["buffs"].getMap()) {
    std::string type = kv.second["category"].getString();
    auto& dst = value["simMapping"]["buffs"][kv.first];
    dst["category"] = fmtstring("$Simulator/%s$", type.c_str());
    if (type == "Status") {
      dst["name"] = value["simMapping"]["status"][kv.first];
    } else {
      std::string name, repl;
      std::string realname = kv.second["name"].getString();
      if (kv.second.has("skill")) {
        auto& skill = kv.second["skill"];
        std::string id = skill[0].getString();
        if (!data["skillMap"].has(id)) {
          Logger::log("Unknown skill: %s", id.c_str());
        } else {
          std::string power = data["skillMap"][id].getString();
          name = powersEn[power + "_name"];
          repl = powers[power + "_name"];
          if (skill.length() > 1) {
            char rune = skill[1].getString()[0];
            name.append(" - ");
            name.append(attrsEn.getfmt("NameRune_%c#%s", rune - 'a' + 'A', power.c_str()));
            repl.append(" - ");
            repl.append(attrs.getfmt("NameRune_%c#%s", rune - 'a' + 'A', power.c_str()));
          }
        }
      } else if (kv.second.has("passive")) {
        std::string id = kv.second["passive"].getString();
        if (!data["skillMap"].has(id)) {
          Logger::log("Unknown passive: %s", id.c_str());
        } else {
          std::string power = data["skillMap"][id].getString();
          name = powersEn[power + "_name"];
          repl = powers[power + "_name"];
        }
      } else if (kv.second.has("set")) {
        std::string id = kv.second["set"][0].getString();
        int count = kv.second["set"][1].getInteger();
        if (!data["setMap"].has(id)) {
          Logger::log("Unknown item set: %s", id.c_str());
        } else {
          id = data["setMap"][id].getString();
          name = setsEn[id];
          repl = sets[id];
          if (name != realname) {
            std::string suffix = fmtstring(" (%d)", count);
            name.append(suffix);
            repl.append(suffix);
          }
        }
      } else if (kv.second.has("stat")) {
        std::string id;
        if (kv.second["stat"].type() == json::Value::tString) {
          id = kv.second["stat"].getString();
        } else {
          id = kv.second["stat"][0].getString();
        }
        if (!data["legendaryMap"].has(id)) {
          Logger::log("Unknown item: %s", id.c_str());
        } else {
          id = data["legendaryMap"][id].getString();
          name = itemsEn[id];
          repl = items[id];
        }
      }
      if (realname == name) {
        dst["name"] = repl;
      } else {
        dst["name"] = fmtstring("$Simulator/%s$", realname.c_str());
        data["stringlist"]["Simulator"][realname]["text"] = realname;
        data["stringlist"]["Simulator"][realname]["tip"] = type + " buff";
      }
    }
  }

  StoreJson(value, data);
}

void ListStrings(json::Value& value, json::Value& data) {
  for (auto& kv : value.getMap()) {
    if (kv.second.type() == json::Value::tObject) {
      for (auto& skv : kv.second.getMap()) {
        data["stringlist"][kv.first][skv.first]["text"] = skv.first;
      }
    } else {
      data["stringlist"]["Global"][kv.first]["text"] = kv.first;
    }
  }
  value.clear();
}

static struct {
  std::string name;
  JsonModifier func;
} fileList[] = {
    {"account.js", StoreJson},
    {"buffs.js", TranslateJson},
    {"classes.js", TranslateJson},
    {"extraitems.js", GenerateExtraItems},
    {"gems.js", GenerateGems},
    {"itemflavor.js", GenerateFlavor},
    {"items.js", GenerateItems},
    {"itemsets.js", GenerateItemSets},
    {"skills.js", GenerateSkills},
    {"skilltips.js", GenerateSkillTips},
    {"slots.js", TranslateJson},
    {"stats.js", TranslateJson},
    {"uistats.js", ListUiStats},
    {"simbuffs.js", GenerateSimBuffs},
    {"strings.js", ListStrings},
    {"skilldata.js", ListStrings},
};

void FormatLocale() {
  json::Value data;
  json::parse(File("locale_base/d3data.js"), data, json::mJS);

  data["skillFix"]["wallofdeath"] = "wallofzombies";
  data["skillFix"]["swamplandattunement"] = "physicalattunement";

  for (auto& pair : fileList) {
    json::Value value;
    std::string cbf;
    json::parse(File("locale_base" / pair.name), value, json::mJSCall, &cbf);
    pair.func(value, data);
    if (value.type() != json::Value::tUndefined) {
      json::write(File("locale" / pair.name, "w"), value, json::mJSCall, cbf.c_str());
    }
  }

  if (data.has("stringlist")) {
    json::write(File("locale/stringlist.js", "w"), data["stringlist"], json::mJSON);
  }
}

void CompareJson(json::Value& lhs, json::Value& rhs, json::Value& output) {
  std::string lstr, rstr;
  if (rhs.type() != lhs.type()) return;
  switch (lhs.type()) {
  case json::Value::tString:
    lstr = lhs.getString();
    rstr = rhs.getString();
    if (!lstr.empty() && lstr[0] == '$' && lstr.back() == '$') {
      size_t slash = lstr.find('/');
      std::string cat = lstr.substr(1, slash - 1);
      std::string name = lstr.substr(slash + 1, lstr.length() - slash - 2);
      output[cat][name] = rstr;
    }
    break;
  case json::Value::tArray:
    for (size_t i = 0; i < lhs.length(); ++i) {
      if (i < rhs.length()) {
        CompareJson(lhs[i], rhs[i], output);
      }
    }
    break;
  case json::Value::tObject:
    for (auto it = lhs.begin(); it != lhs.end(); ++it) {
      if (rhs.has(it.key())) {
        CompareJson(*it, rhs[it.key()], output);
      }
    }
    break;
  }
}
void CompareJsonRight(json::Value& lhs, json::Value& rhs, json::Value& output) {
  for (auto& kv : rhs.getMap()) {
    if (kv.second.type() == json::Value::tObject) {
      for (auto& skv : kv.second.getMap()) {
        output[kv.first][skv.first] = skv.second;
      }
    } else {
      output["Global"][kv.first] = kv.second;
    }
  }
}

typedef void(*CompareFunc)(json::Value& lhs, json::Value& rhs, json::Value& output);
static struct {
  std::string name;
  CompareFunc func;
} compareList[] = {
    {"account.js", CompareJson},
    {"buffs.js", CompareJson},
    {"classes.js", CompareJson},
    {"extraitems.js", CompareJson},
    {"gems.js", CompareJson},
    {"itemflavor.js", CompareJson},
    {"items.js", CompareJson},
    {"itemsets.js", CompareJson},
    {"skills.js", CompareJson},
    {"skilltips.js", CompareJson},
    {"slots.js", CompareJson},
    {"stats.js", CompareJson},
    {"uistats.js", CompareJson},
    {"simbuffs.js", CompareJson},
    {"strings.js", CompareJsonRight},
    {"skilldata.js", CompareJsonRight},
};
void CompareLocale() {
  json::Value output;
  for (auto& pair : compareList) {
    json::Value lhs, rhs;
    File flhs("locale" / pair.name);
    File frhs("locale_diff" / pair.name);
    if (!frhs) continue;
    if (flhs) json::parse(flhs, lhs, json::mJSCall);
    json::parse(frhs, rhs, json::mJSCall);
    pair.func(lhs, rhs, output);
  }
  json::write(File("locale_diff/stringlist.js", "w"), output, json::mJSON);
}
