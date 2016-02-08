#include "miner.h"
#include "description.h"
#include "strings.h"
#include "regexp.h"
#include "affixes.h"
#include "types/StringList.h"
#include "types/SkillKit.h"
#include "types/Actor.h"
#include "types/Recipe.h"
#include "powertag.h"
#include <algorithm>
#include <locale>
#include <cctype>
#include <iostream>

size_t totalLen(std::vector<std::string> const& list) {
  size_t sum = 0;
  for (auto& str : list) sum += str.length();
  return sum;
}

struct FormatData {
  DictionaryRef itemPowers;
  DictionaryRef powers;
  DictionaryRef attributes;
  DictionaryRef items;
  DictionaryRef itemFlavor;
  DictionaryRef itemSets;
  std::map<uint32, std::string> actorImages;
  std::set<uint32> charSkills;
  std::set<uint32> charTraits;
  std::map<uint32, std::vector<uint32>> recipes;

  void load(DictionaryRef& dict, char const* path) {
    Logger::item(path);
    dict = Strings::list(path);
  }

  FormatData() {
    Logger::begin(6, "Loading StringLists");
    load(itemPowers, "ItemPassivePowerDescriptions");
    load(attributes, "AttributeDescriptions");
    load(powers, "Powers");
    load(items, "Items");
    load(itemFlavor, "ItemFlavor");
    load(itemSets, "ItemSets");
    Logger::end();

    for (auto& kit : SnoLoader::All<SkillKit>()) {
      std::vector<std::string> skills, traits;
      for (auto& skill : kit->x20_ActiveSkillEntries) {
        charSkills.insert(skill.x00_PowerSno);
      }
      for (auto& trait : kit->x10_TraitEntries) {
        charTraits.insert(trait.x00_PowerSno);
      }
    }

    for (auto& rcp : SnoLoader::All<Recipe>()) {
      auto& dst = recipes[rcp->x0C_ItemSpecifierData.x00_ItemsGameBalanceId];
      for (int i = 0; i < rcp->x0C_ItemSpecifierData.x04 && i < 6; ++i) {
        dst.push_back(rcp->x0C_ItemSpecifierData.x08_GameBalanceIds[i]);
      }
    }

    for (auto& actor : SnoLoader::All<Actor>()) {
      std::string icons = "";
      for (int i = 0; i < 6; ++i) {
        if (i) icons.push_back(',');
        icons.append(fmtstring("%u,%u",
          actor->x310_InventoryImages[i].x00,
          actor->x310_InventoryImages[i].x04));
      }
      if (icons != "0,0,0,0,0,0,0,0") {
        actorImages[actor->x000_Header.id] = icons;
      }
    }
  }
  ~FormatData() {
  }

  static FormatData& get() {
    static FormatData instance;
    return instance;
  }
};

uint32 PowerAttribute() {
  static uint32 attr = fixAttrId(1270, true);
  return attr;
}

void parseItem(GameBalance::Type::Item const& item, json::Value& to, bool html) {
  FormatData& stl = FormatData::get();
  std::string id = item.x000_Text;
  if (!stl.items.has(id)) return;
  json::Value& dst = to[id];

  if (id == "Lewis_Test_Dagger") {
    int asdf = 0;
  }

  std::vector<AttributeSpecifier> attrs[2];
  std::set<uint32> powers;
  for (auto& attr : item.x1F8_AttributeSpecifiers) {
    if (attr.x00_Type != -1) {
      attrs[GameAffixes::isSecondary(attr.x00_Type)].emplace_back(attr);
      if (attr.x00_Type == PowerAttribute()) {
        powers.insert(attr.x04_Param);
      }
    }
  }
  auto recipe = stl.recipes.find(HashNameLower(id));
  if (recipe != stl.recipes.end()) {
    for (uint32 affixId : recipe->second) {
      AffixValue const& affix = GameAffixes::getAffix(affixId, true);
      for (auto& attr : affix.attributes) {
        if (attr.type != -1) {
          attrs[GameAffixes::isSecondary(attr.type)].push_back(attr);
          if (attr.type == PowerAttribute()) {
            powers.insert(attr.param);
          }
        }
      }
    }
  }
  if (item.x4B8_AttributeSpecifier.x00_Type != -1) {
    attrs[GameAffixes::isSecondary(item.x4B8_AttributeSpecifier.x00_Type)].emplace_back(item.x4B8_AttributeSpecifier);
    if (item.x4B8_AttributeSpecifier.x00_Type == PowerAttribute()) {
      powers.insert(item.x4B8_AttributeSpecifier.x04_Param);
    }
  }
  if (item.x4D0_AttributeSpecifier.x00_Type != -1) {
    attrs[GameAffixes::isSecondary(item.x4D0_AttributeSpecifier.x00_Type)].emplace_back(item.x4D0_AttributeSpecifier);
    if (item.x4D0_AttributeSpecifier.x00_Type == PowerAttribute()) {
      powers.insert(item.x4D0_AttributeSpecifier.x04_Param);
    }
  }

  for (auto& pid : powers) {
    char const* name = Power::name(pid);
    if (name) dst["powers"].append(name);
  }

  dst["name"] = stl.items[id];
  uint32 type = item.x10C_ItemTypesGameBalanceId;
  while (true) {
    uint32 parent = GameAffixes::itemTypeParent(type);
    if (!parent || parent == -1) break;
    type = parent;
  }
  uint32 setbonus = item.x170_SetItemBonusesGameBalanceId;
  char const* setname = SnoManager::gameBalance()[setbonus];
  if (setname) {
    static re::Prog getname(R"((.*) \([0-9]+\))");
    std::string name = getname.replace(setname, "\\1");
    dst["set"] = stl.itemSets[name];
  }
  static uint32 armorIds[] = {
    HashNameLower("Armor"),
    HashNameLower("Jewelry"),
    HashNameLower("Offhand"),
    HashNameLower("Socketable"),
    HashNameLower("Weapon"),
  };
  if (type != armorIds[0] && type != armorIds[1] && type != armorIds[2] && type != armorIds[3] && type != armorIds[4]) {
    dst["trivial"] = true;
  }
  if (stl.actorImages.count(item.x108_ActorSno)) {
    dst["icon"] = stl.actorImages[item.x108_ActorSno];
  }
  if (stl.itemFlavor.has(id)) {
    dst["flavor"] = stl.itemFlavor[id];
  }
  for (int type = 0; type < 2; ++type) {
    auto bonuses = GameAffixes::format(attrs[type], html ? FormatHTML : FormatNone);
    if (bonuses.empty()) continue;
    auto& attrDst = dst[type ? "secondary" : "primary"];
    for (auto& str : bonuses) {
      attrDst.append(str);
    }
  }
  for (int group = 0; group < 6; ++group) {
    uint32 groupId = (&item.x3E4_AffixGroupGameBalanceId)[group];
    if (groupId == -1) continue;
    auto values = GameAffixes::getGroup(groupId, item.x10C_ItemTypesGameBalanceId);
    if (values.empty()) continue;
    bool secondary = GameAffixes::isSecondary(values[0].attributes[0].type);
    auto& attrDst = dst[secondary ? "secondary" : "primary"];
    if (values.size() == 1) {
      auto effects = values[0].format(html ? FormatHTML : FormatNone);
      for (auto& str : effects) {
        attrDst.append(str);
      }
      continue;
    }
    json::Value groupVal;
    for (auto& value : values) {
      auto effects = value.format(html ? FormatHTML : FormatNone);
      if (!effects.empty()) {
        groupVal.append(join(effects, " and "));
      }
    }
    if (groupVal.length()) {
      attrDst.append(groupVal);
    }
  }
  if (item.x128) dst["primary"].append(fmtstring("+%d Random Magic Properties", item.x128));
  if (item.x12C) dst["secondary"].append(fmtstring("+%d Random Magic Properties", item.x12C));
}
void parseSetBonus(GameBalance::Type::SetItemBonusTableEntry const& bonus, json::Value& to, bool html) {
  static re::Prog getname(R"((.*) \([0-9]+\))");
  FormatData& stl = FormatData::get();

  std::string id = bonus.x000_Text;
  id = getname.replace(id, "\\1");

  to[id]["name"] = stl.itemSets[id];
  std::string key = fmtstring("%d", bonus.x10C);

  std::set<uint32> powers;
  std::vector<AttributeSpecifier> attrs;
  for (auto& attr : bonus.x110_AttributeSpecifiers) {
    if (attr.x00_Type >= 0) {
      attrs.emplace_back(attr);
      if (attr.x00_Type == PowerAttribute()) {
        powers.insert(attr.x04_Param);
      }
    }
  }
  auto bonuses = GameAffixes::format(attrs, html ? FormatHTML : FormatNone);
  for (auto& str : bonuses) {
    to[id][key].append(str);
  }
  for (auto& pid : powers) {
    char const* name = Power::name(pid);
    if (name) to[id]["powers"].append(name);
  }
}

void parsePower(Power::Type const& power, json::Value& to, bool html) {
  FormatData& stl = FormatData::get();
  std::string id = Power::name(power.x000_Header.id);
  json::Value& dst = to[id];
  PowerTag* tag = PowerTags::get(id);
  AttributeMap attr = GameAffixes::defaultMap();
  attr.emplace("sLevel", 1);
  dst["name"] = stl.powers[id + "_name"];
  std::string stats;
  if (stl.charTraits.count(power.x000_Header.id)) {
    stats = stl.powers[id + "_var_stats"];
    dst["flavor"] = stl.powers[id + "_desc"];
  } else {
    stats = stl.powers[id + "_desc"];
  }
  dst["desc"] = FormatDescription(stats, html ? FormatHTML : FormatNone, attr, tag);
  if (stl.charSkills.count(power.x000_Header.id)) {
    for (char rune = 'a'; rune <= 'e'; ++rune) {
      std::string nameid = fmtstring("NameRune_%c#%s", rune - 'a' + 'A', id.c_str());
      std::string descid = fmtstring("Rune_%c#%s", rune - 'a' + 'A', id.c_str());
      if (stl.attributes.has(nameid) || stl.attributes.has(descid)) {
        auto& rval = dst[fmtstring("rune_%c", rune)];
        if (stl.attributes.has(nameid)) {
          rval.append(stl.attributes[nameid]);
        }
        if (stl.attributes.has(descid)) {
          AttributeMap rattr = attr;
          rattr.emplace(fmtstring("Rune_%c", rune - 'a' + 'A'), 1.0);
          rval.append(FormatDescription(stl.attributes[descid], html ? FormatHTML : FormatNone, rattr, tag));
        }
      }
    }
  }
  if (tag) {
    for (auto& kv : tag->formulas()) {
      std::string comment = tag->comment(kv.first);
      if (comment.empty()) {
        dst[kv.first] = kv.second;
      } else {
        dst[kv.first].append(kv.second);
        dst[kv.first].append(comment);
      }
    }
  }
}

std::string firstKey(json::Value const& value) {
  switch (value.type()) {
  case json::Value::tString:
    return value.getString();
  case json::Value::tInteger:
    return fmtstring("%d", value.getInteger());
  case json::Value::tNumber:
    return fmtstring("%lf", value.getNumber());
  case json::Value::tArray:
  case json::Value::tObject:
    for (auto& sub : value) {
      std::string key = firstKey(sub);
      if (!key.empty()) return key;
    }
  }
  return "";
}
void toObject(json::Value& value) {
  json::Value tmp(json::Value::tObject);
  for (auto& val : value) {
    tmp[firstKey(val)] = val;
  }
  value = tmp;
}
void cleanup(json::Value& value) {
  json::Value tmp(json::Value::tObject);
  for (auto& it : value.getMap()) {
    if (it.second.type() != json::Value::tUndefined) {
      tmp[it.first] = it.second;
    }
  }
  if (tmp.getMap().empty()) {
    value.clear();
  } else {
    value = tmp;
  }
}

bool jsonEqual(json::Value& lhs, json::Value& rhs) {
  switch (lhs.type()) {
  case json::Value::tString: return lhs.getString() == rhs.getString();
  case json::Value::tInteger: return lhs.getInteger() == rhs.getInteger();
  case json::Value::tNumber: return lhs.getNumber() == rhs.getNumber();
  case json::Value::tBoolean: return lhs.getBoolean() == rhs.getBoolean();
  default: return true;
  }
}
void jsonCompare(json::Value& lhs, json::Value& rhs, bool noArrays) {
  if (lhs.type() != rhs.type()) return;
  if (lhs.type() == json::Value::tArray) {
    if (noArrays) {
      toObject(lhs);
      toObject(rhs);
    } else {
      bool same = true;
      for (size_t i = 0; i < lhs.length() && i < rhs.length(); ++i) {
        jsonCompare(lhs[i], rhs[i], noArrays);
        if (lhs[i].type() != json::Value::tUndefined) {
          same = false;
        }
      }
      if (same && lhs.length() == rhs.length()) {
        lhs.clear();
        rhs.clear();
      }
      return;
    }
  }
  if (lhs.type() == json::Value::tObject) {
    auto lit = lhs.begin();
    auto rit = rhs.begin();
    while (lit != lhs.end() && rit != rhs.end()) {
      if (lit.key() == rit.key()) {
        jsonCompare(*lit, *rit, noArrays);
        ++lit;
        ++rit;
      } else if (lit.key() < rit.key()) {
        ++lit;
      } else {
        ++rit;
      }
    }
    cleanup(lhs);
    cleanup(rhs);
  } else if (jsonEqual(lhs, rhs)) {
    lhs.clear();
    rhs.clear();
  }
}


double strdist(std::string const& slhs, std::string const& srhs) {
  auto lhs = split(slhs);
  auto rhs = split(srhs);
  size_t n = lhs.size();
  size_t m = rhs.size();
  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      if (lhs[i] == rhs[j]) {
        dp[i + 1][j + 1] = dp[i][j] + lhs[i].length();
      } else {
        dp[i + 1][j + 1] = std::max(dp[i + 1][j], dp[i][j + 1]);
      }
    }
  }
  return 1.0 * dp[n][m] / std::max(totalLen(lhs), totalLen(rhs));
}
std::string strdiff(std::string const& slhs, std::string const& srhs) {
  auto lhs = split(slhs);
  auto rhs = split(srhs);
  size_t n = lhs.size();
  size_t m = rhs.size();
  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
  for (int i = n - 1; i >= 0; --i) {
    for (int j = m - 1; j >= 0; --j) {
      if (lhs[i] == rhs[j]) {
        dp[i][j] = dp[i + 1][j + 1] + lhs[i].length();
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
        if (!result.empty()) result.push_back(' ');
        result.append("<span class=\"bg-red\">");
        result.append(join(lhs.begin() + pi, lhs.begin() + ci));
        result.append("</span>");
      }
      if (pj < cj) {
        if (!result.empty()) result.push_back(' ');
        result.append("<span class=\"bg-green\">");
        result.append(join(rhs.begin() + pj, rhs.begin() + cj));
        result.append("</span>");
      }
      if (!result.empty()) result.push_back(' ');
      result.append(lhs[ci]);
      pi = ++ci;
      pj = ++cj;
    } else if (dp[ci][cj] == dp[ci + 1][cj]) {
      ++ci;
    } else {
      ++cj;
    }
  }
  if (pi < n) {
    if (!result.empty()) result.push_back(' ');
    result.append("<span class=\"bg-red\">");
    result.append(join(lhs.begin() + pi, lhs.end()));
    result.append("</span>");
  }
  if (pj < m) {
    if (!result.empty()) result.push_back(' ');
    result.append("<span class=\"bg-green\">");
    result.append(join(rhs.begin() + pj, rhs.end()));
    result.append("</span>");
  }
  return result;
}
typedef std::vector<std::pair<json::Value const*, json::Value const*>> MergedArrays;
MergedArrays jsonMerge(json::Value const& lhs, json::Value const& rhs, bool exact = false);
size_t jsonLength(json::Value const& val) {
  if (val.type() != json::Value::tArray) {
    return firstKey(val).length();
  }
  size_t sum = 0;
  for (auto& sub : val) {
    sum += jsonLength(sub);
  }
  return sum;
}
double jsonDist(json::Value const& lhs, json::Value const& rhs) {
  if (lhs.type() != rhs.type()) return 0;
  if (lhs.type() != json::Value::tArray) {
    return strdist(firstKey(lhs), firstKey(rhs));
  }
  auto list = jsonMerge(lhs, rhs);
  double sum = 0;
  for (auto& pp : list) {
    if (pp.first && pp.second) {
      sum += jsonDist(*pp.first, *pp.second) * std::max(jsonLength(*pp.first), jsonLength(*pp.second));
    }
  }
  return sum / std::max((size_t) 1, std::max(jsonLength(lhs), jsonLength(rhs)));
}
MergedArrays jsonMerge(json::Value const& lhs, json::Value const& rhs, bool exact) {
  MergedArrays list;
  for (auto& val : lhs) {
    list.emplace_back(&val, nullptr);
  }
  for (auto& val : rhs) {
    size_t pos = 0;
    while (pos < list.size()) {
      if (!list[pos].second) {
        if (exact) {
          if (firstKey(val) == firstKey(*list[pos].first)) break;
        } else {
          if (jsonDist(val, *list[pos].first) > 0.5) break;
        }
      }
      ++pos;
    }
    if (pos < list.size()) {
      list[pos].second = &val;
    } else {
      list.emplace_back(nullptr, &val);
    }
  }
  return list;
}
std::string firstKeyPtr(json::Value const* val) {
  return (val ? firstKey(*val) : "");
}

std::vector<std::string> mergeKeys(json::Value const& lhs, json::Value const& rhs, std::set<std::string> const& excl) {
  std::set<std::string> keys;
  for (auto& kv : lhs.getMap()) {
    if (excl.find(kv.first) == excl.end()) keys.insert(kv.first);
  }
  for (auto& kv : rhs.getMap()) {
    if (excl.find(kv.first) == excl.end()) keys.insert(kv.first);
  }
  return std::vector<std::string>(keys.begin(), keys.end());
}

void diff(File& file, json::Value const& lhs, json::Value const& rhs, std::vector<std::string> const& order, bool links) {
  json::Value lhs_trim = lhs;
  json::Value rhs_trim = rhs;
  jsonCompare(lhs_trim, rhs_trim);
  auto keys = mergeKeys(lhs_trim, rhs_trim);

  std::set<std::string> excl;
  for (auto& v : order) excl.insert(v);
  for (auto& key : keys) {
    file.printf("  <a name=\"%s\"></a>\n", key.c_str());
    file.printf("  <div");
    if (lhs[key].has("trivial") || rhs[key].has("trivial")) {
      file.printf(" class=\"trivial\"");
    }
    file.printf(">\n");
    if (links) {
      file.printf("   <i><a href=\"/powers/%s\">%s</a></i>\n", key.c_str(), key.c_str());
    } else {
      file.printf("   <i>%s</i>\n", key.c_str());
    }
    auto subkeys = order;
    auto merged = mergeKeys(lhs[key], rhs[key], excl);
    subkeys.insert(subkeys.end(), merged.begin(), merged.end());
    for (size_t i = 0; i < subkeys.size(); ++i) {
      auto sub = subkeys[i];
      if (sub == "trivial" || !lhs[key].has(sub) && !rhs[key].has(sub)) continue;
      json::Value const& lv = lhs[key][sub];
      json::Value const& rv = rhs[key][sub];
      if (sub == "icon") {
        std::string id = firstKey(rv);
        if (id.empty()) id = firstKey(lv);
        file.printf("   <span class=\"label\">icon</span>: <span class=\"icon\" icons=\"%s\">(show)</span><br/>\n", id);
        continue;
      }
      if (lv.type() == json::Value::tArray || rv.type() == json::Value::tArray) {
        MergedArrays list;
        if (i < order.size()) {
          list = jsonMerge(lv, rv, sub == "powers");
        } else {
          for (auto& val : lv) {
            list.emplace_back(&val, nullptr);
          }
          for (size_t i = 0; i < rv.length(); ++i) {
            if (i < list.size()) {
              list[i].second = &rv[i];
            } else {
              list.emplace_back(nullptr, &rv[i]);
            }
          }
        }
        if (!list.empty() && (i < order.size() || firstKeyPtr(list[0].first) != firstKeyPtr(list[0].second))) {
          file.printf("   <span class=\"label\">%s</span>: <p class=\"indent\">\n", sub.c_str());
          for (auto& lr : list) {
            if (sub == "powers") {
              if (lr.second) {
                file.printf("    <a href=\"/powers/%s\">", firstKey(*lr.second).c_str());
              } else {
                file.printf("    <a href=\"/powers/%s\">", firstKey(*lr.first).c_str());
              }
            } else {
              file.printf("    ");
            }
            json::Value lval, rval;
            if (lr.first && lr.second) {
              lval = *lr.first;
              rval = *lr.second;
            } else if (lr.first) {
              lval = *lr.first;
              rval.setType(lval.type());
            } else {
              rval = *lr.second;
              lval.setType(rval.type());
            }
            if (sub != "powers" && lval.type() == json::Value::tArray) {
              while (rval.length() < lval.length()) {
                rval.append("");
              }
              while (lval.length() < rval.length()) {
                lval.append("");
              }
              file.printf("One of %d Magic Properties (varies)\n", lval.length());
              file.printf("    <span class=\"indent\">");
              for (size_t i = 0; i < lval.length(); ++i) {
                file.printf("      %s<br/>\n", strdiff(firstKey(lval[i]), firstKey(rval[i])).c_str());
              }
              file.printf("    </span>\n");
            } else {
              file.printf("%s", strdiff(firstKey(lval), firstKey(rval)).c_str());
            }
            if (sub == "powers") {
              file.printf("</a><br/>\n");
            } else if (lval.type() != json::Value::tArray) {
              file.printf("<br/>\n");
            }
          }
          file.printf("   </p>\n");
        }
      } else {
        auto a = firstKey(lv);
        auto b = firstKey(rv);
        if (i < order.size() || a != b) {
          file.printf("   <span class=\"label\">%s</span>: ", sub.c_str());
          file.printf("%s<br/>\n", strdiff(a, b).c_str());
        }
      }
    }
    file.printf("  </div>\n");
  }
}

void makehtml(File& file, json::Value const& val, std::vector<std::string> const& order, bool links) {
  std::set<std::string> excl;
  for (auto& v : order) excl.insert(v);
  for (auto& kv : val.getMap()) {
    auto& key = kv.first;
    auto& item = kv.second;
    if (links) {
      if (item["name"].getString().empty()) continue;
    }
    file.printf("  <a name=\"%s\"></a>\n", key.c_str());
    file.printf("  <div");
    if (item.has("trivial")) {
      file.printf(" class=\"trivial\"");
    }
    file.printf(">\n");
    if (links) {
      file.printf("   <i><a href=\"/powers/%s\">%s</a></i>\n", key.c_str(), key.c_str());
    } else {
      file.printf("   <i>%s</i>\n", key.c_str());
    }
    auto subkeys = order;
    if (!links) {
      for (auto& kv2 : item.getMap()) {
        if (excl.find(kv2.first) == excl.end()) {
          subkeys.push_back(kv2.first);
        }
      }
    }
    for (size_t i = 0; i < subkeys.size(); ++i) {
      auto sub = subkeys[i];
      if (sub == "trivial" || !item.has(sub)) continue;
      json::Value const& lv = item[sub];
      if (sub == "icon") {
        std::string id = firstKey(lv);
        file.printf("   <span class=\"label\">icon</span>: <span class=\"icon\" icons=\"%s\">(show)</span><br/>\n", id);
        continue;
      }
      if (lv.type() == json::Value::tArray) {
        file.printf("   <span class=\"label\">%s</span>: <p class=\"indent\">\n", sub.c_str());
        for (auto& lri : lv) {
          if (sub == "powers") {
            file.printf("    <a href=\"/powers/%s\">", firstKey(lri).c_str());
          } else {
            file.printf("    ");
          }
          if (lri.type() != json::Value::tArray) {
            file.printf("%s", firstKey(lri).c_str());
          } else {
            file.printf("One of %d Magic Properties (varies)\n", lri.length());
            file.printf("    <span class=\"indent\">");
            for (size_t i = 0; i < lri.length(); ++i) {
              file.printf("      %s<br/>\n", firstKey(lri[i]).c_str());
            }
            file.printf("    </span>\n");
          }
          if (sub == "powers") {
            file.printf("</a><br/>\n");
          } else if (lri.type() != json::Value::tArray) {
            file.printf("<br/>\n");
          }
        }
        file.printf("   </p>\n");
      } else {
        file.printf("   <span class=\"label\">%s</span>: %s<br/>\n", sub.c_str(), firstKey(lv).c_str());
      }
    }
    file.printf("  </div>\n");
  }
}

json::Value dumpPower(json::Value const& pow) {
  static std::string SF = "TAG_POWER_SCRIPT_FORMULA_";
  auto& sfd = pow["x438_ScriptFormulaDetails"];
  json::Value res;
  res["id"] = pow["x000_Header"]["id"];
  for (auto& kv : pow["x050_PowerDef"].getMap()) {
    if (kv.first.find("_TagMap") != std::string::npos) {
      for (auto& pv : kv.second.getMap()) {
        auto& dst = res["tags"][pv.first];
        dst = pv.second;
        if (dst.type() == json::Value::tArray && pv.first.find(SF) == 0) {
          int index = atoi(pv.first.c_str() + SF.length());
          if (index < sfd.length()) {
            dst.append(sfd[index]["x000_Text"]);
          }
        }
      }
    }
  }
  return res;
}

namespace MinerPrivate {

  void dumpfile(SnoLoader& loader, std::string const& name, ParseFunc func, std::string const& type) {
    Logger::item(name.c_str());
    File out("diff" / type / name + ".txt", "w");
    json::WriterVisitor writer(out);
    writer.setIndent(2);
    func(loader, name, &writer);
    writer.onEnd();
  }
  void readfile(SnoLoader& loader, std::string const& name, ParseFunc func, json::Value& value) {
    json::BuilderVisitor builder(value);
    func(loader, name, &builder);
    builder.onEnd();
  }

  void fulldiff(SnoLoader& lhs, SnoLoader& rhs,
    ParseFunc func, ListFunc list, std::string const& type)
  {
    std::set<istring> names;
    auto lhs_list = list(lhs);
    auto rhs_list = list(rhs);
    for (auto& x : lhs_list) names.insert(x);
    for (auto& x : rhs_list) names.insert(x);
    Logger::begin(names.size(), ("Comparing " + type).c_str());
    auto li = lhs_list.begin(), ri = rhs_list.begin();
    while (li != lhs_list.end() || ri != rhs_list.end()) {
      if (li == lhs_list.end()) {
        dumpfile(rhs, *ri++, func, type);
      } else if (ri == rhs_list.end()) {
        dumpfile(lhs, *li++, func, type);
      } else {
        if (*li == *ri) {
          auto name = *li++;
          ri++;
          Logger::item(name.c_str());
          json::Value lval, rval;
          readfile(lhs, name, func, lval);
          readfile(rhs, name, func, rval);
          jsonCompare(lval, rval, false);
          if (lval.type() != json::Value::tUndefined) {
            json::write(File("diff" / type / name + "_lhs.txt", "w"), lval);
            json::write(File("diff" / type / name + "_rhs.txt", "w"), rval);
          }
        } else if (*li < *ri) {
          dumpfile(lhs, *li++, func, type);
        } else {
          dumpfile(rhs, *ri++, func, type);
        }
      }
    }
    Logger::end();
  }
}
