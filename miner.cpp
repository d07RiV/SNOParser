#include "miner.h"
#include "description.h"
#include "strings.h"
#include "regexp.h"
#include "types/StringList.h"
#include "types/SkillKit.h"
#include "types/Actor.h"
#include "powertag.h"
#include <algorithm>
#include <locale>
#include <cctype>
#include <iostream>

AttributeValue ParseValue(GameBalance::Type::AttributeSpecifier const& attr) {
  std::vector<double> vals;
  for (size_t i = 1; i < attr.x08_Data.size(); ++i) {
    if (attr.x08_Data[i - 1].value == 6) {
      vals.push_back(*(float*)&attr.x08_Data[i].value);
    }
  }
  switch (vals.size()) {
  case 0: return AttributeValue();
  case 1: return AttributeValue(vals[0]);
  case 2: return AttributeValue(vals[0], vals[1]);
  default: return AttributeValue(vals[0] / vals[2], vals[1] / vals[2]);
  }
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
  json::Value affixes;
  json::Value missing;

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
    json::parse(File("affixes.js"), affixes);
    File fm("missing.js");
    if (fm) json::parse(fm, missing);

    for (auto& kit : SnoLoader::All<SkillKit>()) {
      std::vector<std::string> skills, traits;
      for (auto& skill : kit->x20_ActiveSkillEntries) {
        charSkills.insert(skill.x00_PowerSno);
      }
      for (auto& trait : kit->x10_TraitEntries) {
        charTraits.insert(trait.x00_PowerSno);
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
    if (!missing.getMap().empty()) {
      json::write(File("missing.js", "w"), missing);
    }
  }

  static FormatData& get() {
    static FormatData instance;
    return instance;
  }
};

#ifdef PTR
int fixAttrId(int id) {
  return id;
}
#else
int fixAttrId(int id) {
  if (id >= 677) {
    return id + 5;
  } else if (id >= 165) {
    return id + 3;
  } else {
    return id;
  }
}
#endif

std::vector<std::string> FormatBonus(std::vector<GameBalance::Type::AttributeSpecifier const*> const& attrs, bool html, std::string const& tip = "") {
  FormatData& stl = FormatData::get();
  std::vector<std::string> result;
  std::map<std::string, AttributeMap> aggregate;
  //std::cerr << tip << std::endl;
  for (auto& attr : attrs) {
    std::string affix = fmtstring("%d", fixAttrId(attr->x00_Type));
    if (!stl.affixes["Affixes"].has(affix)) {
      stl.missing[affix] = tip;
      continue;
    }
    affix = stl.affixes["Affixes"][affix].getString();
    if (affix == "{Power}") {
      char const* power = Power::name(attr->x04_Param);
      if (power && stl.itemPowers.has(power)) {
        affix = stl.itemPowers[power];
      } else {
        continue;
      }
      AttributeMap map;
      map.emplace("value1", ParseValue(*attr));
      result.push_back(FormatDescription(affix, html, map));
    } else if (affix[0] == '&') {
      char const* power = Power::name(attr->x04_Param);
      if (!power) continue;
      std::string name = fmtstring("%s_name", power);
      if (!stl.powers.has(name)) continue;
      name = stl.powers[name];
      affix = affix.substr(1);
      if (!stl.attributes.has(affix)) continue;
      affix = stl.attributes[affix];
      AttributeMap map;
      map.emplace("value1", name);
      map.emplace("value2", ParseValue(*attr));
      if (strlower(affix).find("value3") != std::string::npos) {
        std::string pwr = strlower(power);
        for (auto& kv : stl.affixes["ResourceX"].getMap()) {
          if (pwr.find(kv.first) != std::string::npos) {
            map.emplace("value3", kv.second.getString());
            break;
          }
        }
      }
      result.push_back(FormatDescription(affix, html, map));
    } else if (affix[0] == '@') {
      affix = affix.substr(1);
      AttributeMap map;
      map.emplace("value", ParseValue(*attr));
      result.push_back(FormatDescription(affix, html, map));
    } else {
      static re::Prog subst(R"(\{(\w+)\})");
      affix = subst.replace(affix, [stl, attr](re::Match const& m) {
        std::string key = m.group(1);
        std::string param = fmtstring("%d", attr->x04_Param);
        return stl.affixes[key][param].getString();
      });
      size_t dollar = affix.find('$');
      if (dollar != std::string::npos) {
        int index = atoi(affix.substr(dollar + 1).c_str());
        affix.resize(dollar);
        aggregate[affix][index ? fmtstring("value%d", index) : "value"] = ParseValue(*attr);
      } else {
        if (!stl.attributes.has(affix)) continue;
        affix = stl.attributes[affix];
        AttributeMap map;
        map.emplace("value", ParseValue(*attr));
        result.push_back(FormatDescription(affix, html, map));
      }
    }
  }
  for (auto& am : aggregate) {
    if (!stl.attributes.has(am.first)) continue;
    result.push_back(FormatDescription(stl.attributes[am.first], html, am.second));
  }
  return result;
}

void parseItem(GameBalance::Type::Item const& item, json::Value& to, bool html) {
  FormatData& stl = FormatData::get();
  std::string id = item.x000_Text;
  if (!stl.items.has(id)) return;
  json::Value& dst = to[id];

  std::vector<GameBalance::Type::AttributeSpecifier const*> attrs;
  std::set<uint32> powers;
  for (auto& attr : item.x1F8_AttributeSpecifiers) {
    if (attr.x00_Type >= 0) {
      attrs.push_back(&attr);
      if (fixAttrId(attr.x00_Type) == 1270) {
        powers.insert(attr.x04_Param);
      }
    }
  }
  if (item.x4B8_AttributeSpecifier.x00_Type >= 0) {
    attrs.push_back(&item.x4B8_AttributeSpecifier);
    if (fixAttrId(item.x4B8_AttributeSpecifier.x00_Type) == 1270) {
      powers.insert(item.x4B8_AttributeSpecifier.x04_Param);
    }
  }
  if (item.x4D0_AttributeSpecifier.x00_Type >= 0) {
    attrs.push_back(&item.x4D0_AttributeSpecifier);
    if (fixAttrId(item.x4D0_AttributeSpecifier.x00_Type) == 1270) {
      powers.insert(item.x4D0_AttributeSpecifier.x04_Param);
    }
  }

  //if (id != "Unique_Gem_018_x1") return;
  for (auto& pid : powers) {
    char const* name = Power::name(pid);
    if (name) dst["powers"].append(name);
  }

  dst["name"] = stl.items[id];
  if (stl.actorImages.count(item.x108_ActorSno)) {
    dst["icon"] = stl.actorImages[item.x108_ActorSno];
  }
  auto bonuses = FormatBonus(attrs, html, stl.items[id]);
  if (stl.itemFlavor.has(id)) {
    dst["flavor"] = stl.itemFlavor[id];
  }
  for (auto& str : bonuses) {
    dst["attributes"].append(str);
  }
}
void parseSetBonus(GameBalance::Type::SetItemBonusTableEntry const& bonus, json::Value& to, bool html) {
  static re::Prog getname(R"((.*) \([0-9]+\))");
  FormatData& stl = FormatData::get();

  std::string id = bonus.x000_Text;
  id = getname.replace(id, "\\1");

  to[id]["name"] = stl.itemSets[id];
  std::string key = fmtstring("%d", bonus.x10C);

  std::set<uint32> powers;
  std::vector<GameBalance::Type::AttributeSpecifier const*> attrs;
  for (auto& attr : bonus.x110_AttributeSpecifiers) {
    if (attr.x00_Type >= 0) {
      attrs.push_back(&attr);
      if (fixAttrId(attr.x00_Type) == 1270) {
        powers.insert(attr.x04_Param);
      }
    }
  }
  auto bonuses = FormatBonus(attrs, html, stl.itemSets[id]);
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
  AttributeMap attr;
  attr.emplace("Attacks_Per_Second_Total", 1.0);
  attr.emplace("Level", 70.0);
  attr.emplace("Effective_Level", 70.0);
  dst["name"] = stl.powers[id + "_name"];
  std::string stats;
  if (stl.charTraits.count(power.x000_Header.id)) {
    stats = stl.powers[id + "_var_stats"];
    dst["flavor"] = stl.powers[id + "_desc"];
  } else {
    stats = stl.powers[id + "_desc"];
  }
  dst["desc"] = FormatDescription(stats, html, attr, tag);
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
          rval.append(FormatDescription(stl.attributes[descid], html, rattr, tag));
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
void jsonCompare(json::Value& lhs, json::Value& rhs) {
  if (lhs.type() != rhs.type()) return;
  if (lhs.type() == json::Value::tArray) {
    toObject(lhs);
    toObject(rhs);
  }
  if (lhs.type() == json::Value::tObject) {
    auto lit = lhs.begin();
    auto rit = rhs.begin();
    while (lit != lhs.end() && rit != rhs.end()) {
      if (lit.key() == rit.key()) {
        jsonCompare(*lit, *rit);
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

std::vector<std::string> split(std::string const& str) {
  std::vector<std::string> res;
  std::string cur;
  for (char c : str) {
    if (c == ' ') {
      res.push_back(cur);
      cur.clear();
    } else {
      cur.push_back(c);
    }
  }
  res.push_back(cur);
  return res;
}
std::string join(std::vector<std::string> const& list) {
  std::string res;
  for (auto& str : list) {
    if (!res.empty()) res.push_back(' ');
    res.append(str);
  }
  return res;
}
template<class Iter>
std::string join(Iter left, Iter right) {
  std::string res;
  while (left != right) {
    if (!res.empty()) res.push_back(' ');
    res.append(*left++);
  }
  return res;
}
size_t totalLen(std::vector<std::string> const& list) {
  size_t sum = 0;
  for (auto& str : list) sum += str.length();
  return sum;
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
    file.printf("  <div>\n");
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
      if (!lhs[key].has(sub) && !rhs[key].has(sub)) continue;
      json::Value const& lv = lhs[key][sub];
      json::Value const& rv = rhs[key][sub];
      if (sub == "icon") {
        std::string id = firstKey(rv);
        if (id.empty()) id = firstKey(lv);
        file.printf("   <span class=\"label\">icon</span>: <span class=\"icon\" icons=\"%s\">(show)</span><br/>\n", id);
        continue;
      }
      if (lv.type() == json::Value::tArray || rv.type() == json::Value::tArray) {
        std::vector<std::pair<std::string, std::string>> list;
        for (auto& val : lv) {
          list.emplace_back(firstKey(val), "");
        }
        if (i < order.size()) {
          for (auto& val : rv) {
            size_t pos = 0;
            std::string cv = firstKey(val);
            while (pos < list.size()) {
              if (list[pos].second.empty()) {
                if (sub == "powers") {
                  if (cv == list[pos].first) break;
                } else {
                  if (strdist(cv, list[pos].first) > 0.5) break;
                }
              }
              ++pos;
            }
            if (pos < list.size()) {
              list[pos].second = cv;
            } else {
              list.emplace_back("", cv);
            }
          }
        } else {
          for (size_t i = 0; i < rv.length(); ++i) {
            if (i < list.size()) {
              list[i].second = firstKey(rv[i]);
            } else {
              list.emplace_back("", firstKey(rv[i]));
            }
          }
        }
        if (!list.empty() && (i < order.size() || list[0].first != list[0].second)) {
          file.printf("   <span class=\"label\">%s</span>: <p class=\"indent\">\n", sub.c_str());
          for (auto& lr : list) {
            if (sub == "powers") {
              if (!lr.second.empty()) {
                file.printf("    <a href=\"/powers/%s\">", lr.second.c_str());
              } else {
                file.printf("    <a href=\"/powers/%s\">", lr.first.c_str());
              }
            } else {
              file.printf("    ");
            }
            file.printf("%s", strdiff(lr.first, lr.second).c_str());
            if (sub == "powers") {
              file.printf("</a>");
            }
            file.printf("<br/>\n");
          }
          file.printf("   </p>\n");
        }
      } else {
        auto a = firstKey(lv);
        auto b = firstKey(rv);
        if (i < order.size() || a != b) {
          file.printf("   <span class=\"label\">%s</span>: %s<br/>\n", sub.c_str(), strdiff(a, b).c_str());
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
    file.printf("  <div>\n");
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
      if (!item.has(sub)) continue;
      json::Value const& lv = item[sub];
      if (sub == "icon") {
        std::string id = firstKey(lv);
        file.printf("   <span class=\"label\">icon</span>: <span class=\"icon\" icons=\"%s\">(show)</span><br/>\n", id);
        continue;
      }
      if (lv.type() == json::Value::tArray) {
        file.printf("   <span class=\"label\">%s</span>: <p class=\"indent\">\n", sub.c_str());
        for (auto& lri : lv) {
          std::string lr = lri.getString();
          if (sub == "powers") {
            file.printf("    <a href=\"/powers/%s\">", lr.c_str());
          } else {
            file.printf("    ");
          }
          file.printf("%s", lr.c_str());
          if (sub == "powers") {
            file.printf("</a>");
          }
          file.printf("<br/>\n");
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
