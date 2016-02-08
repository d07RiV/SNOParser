#include "affixes.h"
#include "description.h"
#include "strings.h"
#include "powertag.h"
#include "regexp.h"
#include <algorithm>

int fixAttrId(int id, bool reverse) {
  static enum { Unknown, Live, Ptr, Ptr2, Ptr24_1 } version = Unknown;
  if (version == Unknown) {
    SnoFile<GameBalance> gmb("1xx_AffixList");
    for (auto& affix : gmb->x078_AffixTable) {
      if (!strcmp(affix.x000_Text, "1xx_CCReduction 0.1 Legendary")) {
        switch (affix.x260_AttributeSpecifiers[0].x00_Type) {
        case 683: version = Live; break;
        case 689: version = Ptr; break;
        case 690: version = Ptr2; break;
        case 699: version = Ptr24_1; break;
        default: throw Exception("unknown build version");
        }
        break;
      }
    }
  }
  if (reverse) {
    switch (version) {
    case Live:
      if (id >= 677) {
        return id - 5;
      } else if (id >= 168) {
        return id - 3;
      } else {
        return id;
      }
    case Ptr:
      return id;
    case Ptr2:
      if (id >= 677) {
        return id + 1;
      } else {
        return id;
      }
    case Ptr24_1:
      if (id >= 1053) {
        return id + 11;
      } else if (id >= 677) {
        return id + 10;
      } else if (id >= 335) {
        return id + 3;
      } else if (id >= 134) {
        return id + 1;
      } else {
        return id;
      }
    default:
      return id;
    }
  } else {
    switch (version) {
    case Live:
      if (id >= 672) {
        return id + 5;
      } else if (id >= 165) {
        return id + 3;
      } else {
        return id;
      }
    case Ptr:
      return id;
    case Ptr2:
      if (id >= 678) {
        return id - 1;
      } else {
        return id;
      }
    case Ptr24_1:
      if (id >= 1064) {
        return id - 11;
      } else if (id >= 687) {
        return id - 10;
      } else if (id >= 338) {
        return id - 3;
      } else if (id >= 135) {
        return id - 1;
      } else {
        return id;
      }
    default:
      return id;
    }
  }
}

bool GameAffixes::isSecondary(uint32 attr) {
  return instance().affixData_.secondary.count(fixAttrId(attr)) != 0;
}

AttributeSpecifier::AttributeSpecifier(GameBalance::Type::AttributeSpecifier const& attr, AttributeMap const& params)
  : type(attr.x00_Type)
  , param(attr.x04_Param)
{
  if (type != -1) {
    value = ExecFormula(
      reinterpret_cast<uint32 const*>(attr.x08_Data.begin()),
      reinterpret_cast<uint32 const*>(attr.x08_Data.end()),
      params);
  }
}

struct GameAffixes::GameAffix {
  std::set<uint32> itemTypes;
  AffixValue value;
};

GameAffixes::GameAffixes() {
  defaultMap_.emplace("Level", 70.0);
  defaultMap_.emplace("Casting_Speed_Total", 1.0);
  defaultMap_.emplace("Attacks_Per_Second_Total", 1.0);
  defaultMap_.emplace("Effective_Level", 70.0);
  defaultMap_.emplace("iLevel", 72.0);

  SnoFile<GameBalance> affixList("1xx_AffixList");
  for (auto& fx : affixList->x078_AffixTable) {
    uint32 id = HashNameLower(fx.x000_Text);
    GameAffix& affix = affixes_[id];
    for (auto id : fx.x178_GameBalanceIds) {
      if (id != -1) affix.itemTypes.insert(id);
    }
    for (auto id : fx.x190_GameBalanceIds) {
      if (id != -1) affix.itemTypes.insert(id);
    }
    for (auto id : fx.x1F0_GameBalanceIds) {
      if (id != -1) affix.itemTypes.insert(id);
    }
    for (size_t i = 0; i < AffixValue::MaxAttributes; ++i) {
      affix.value.attributes[i] = AttributeSpecifier(fx.x260_AttributeSpecifiers[i], defaultMap_);
    }
    std::sort(affix.value.attributes, affix.value.attributes + AffixValue::MaxAttributes, AttributeSpecifier::less);
    if (fx.x168_AffixGroupGameBalanceId != -1) {
      groups_[fx.x168_AffixGroupGameBalanceId].push_back(&affix);
    }
    if (fx.x16C_AffixGroupGameBalanceId != -1) {
      groups_[fx.x16C_AffixGroupGameBalanceId].push_back(&affix);
    }
  }

  SnoFile<GameBalance> affixListRecipes("AffixList");
  for (auto& fx : affixListRecipes->x078_AffixTable) {
    uint32 id = HashNameLower(fx.x000_Text);
    GameAffix& affix = affixes_[id];
    for (size_t i = 0; i < AffixValue::MaxAttributes; ++i) {
      affix.value.attributes[i] = AttributeSpecifier(fx.x260_AttributeSpecifiers[i], defaultMap_);
    }
    std::sort(affix.value.attributes, affix.value.attributes + AffixValue::MaxAttributes, AttributeSpecifier::less);
  }

  SnoFile<GameBalance> itemTypes("ItemTypes");
  for (auto& type : itemTypes->x018_ItemTypes) {
    if (type.x108_ItemTypesGameBalanceId != -1) {
      itemTypeParent_[HashNameLower(type.x000_Text)] = type.x108_ItemTypesGameBalanceId;
    }
  }

  json::parse(File("affixes.js"), affixData_.raw);
  for (auto& kv : affixData_.raw["Affixes"].getMap()) {
    affixData_.types[atoi(kv.first.c_str())] = kv.second.getString();
  }
  for (auto& kv : affixData_.raw["Secondary"].getMap()) {
    affixData_.secondary.insert(atoi(kv.first.c_str()));
  }
}

AffixValue const& GameAffixes::getAffix(uint32 id, bool recipe) {
  if (recipe) {
    auto it = instance().affixesRecipe_.find(id);
    if (it != instance().affixesRecipe_.end()) {
      return it->second.value;
    }
  }
  return instance().affixes_[id].value;
}
std::vector<AffixValue> GameAffixes::getGroup(uint32 id, uint32 itemType) {
  auto& itemTypeParent = instance().itemTypeParent_;
  std::vector<uint32> itemTypes;
  while (itemType != -1) {
    itemTypes.push_back(itemType);
    auto it = itemTypeParent.find(itemType);
    if (it == itemTypeParent.end()) break;
    itemType = it->second;
  }
  std::map<std::pair<uint32, uint32>, AffixValue*> values;
  for (GameAffix* affix : instance().groups_[id]) {
    bool hasType = false;
    for (auto& type : itemTypes) {
      if (affix->itemTypes.count(type)) {
        hasType = true;
        break;
      }
    }
    if (hasType) {
      auto key = std::make_pair(affix->value.attributes[0].type, affix->value.attributes[0].param);
      auto it = values.find(key);
      if (it == values.end() || AffixValue::less(*it->second, affix->value)) {
        values[key] = &affix->value;
      }
    }
  }
  std::vector<AffixValue> result;
  for (auto& kv : values) {
    result.push_back(*kv.second);
  }
  return result;
}

GameAffixes& GameAffixes::instance() {
  static GameAffixes inst;
  return inst;
}

std::vector<std::string> GameAffixes::format(AttributeSpecifier const* begin, AttributeSpecifier const* end, FormatFlags flags) {
  auto& affixData = instance().affixData_;
  AttributeMap& defaultMap = instance().defaultMap_;
  DictionaryRef itemPowers = Strings::list("ItemPassivePowerDescriptions");
  DictionaryRef powers = Strings::list("Powers");
  DictionaryRef attributes = Strings::list("AttributeDescriptions");

  std::vector<std::string> result;
  std::map<std::string, AttributeMap> aggregate;
  for (; begin < end; ++begin) {
    uint32 type = fixAttrId(begin->type);
    if (!affixData.types.count(type)) continue;
    std::string affix = affixData.types[type];
    if (affix == "{Power}") {
      char const* power = Power::name(begin->param);
      if (power && itemPowers.has(power)) {
        affix = itemPowers[power];
      } else {
        continue;
      }
      AttributeMap map = defaultMap;
      map.emplace("value1", begin->value);
      result.push_back(FormatDescription(affix, flags, map, PowerTags::getraw(begin->param)));
    } else if (affix[0] == '&') {
      char const* power = Power::name(begin->param);
      if (!power) continue;
      std::string name = fmtstring("%s_name", power);
      if (!powers.has(name)) continue;
      name = powers[name];
      affix = affix.substr(1);
      if (!attributes.has(affix)) continue;
      affix = attributes[affix];
      AttributeMap map = defaultMap;
      map.emplace("value1", name);
      map.emplace("value2", begin->value);
      if (strlower(affix).find("value3") != std::string::npos) {
        std::string pwr = strlower(power);
        for (auto& kv : affixData.raw["ResourceX"].getMap()) {
          if (pwr.find(kv.first) != std::string::npos) {
            map.emplace("value3", kv.second.getString());
            break;
          }
        }
      }
      result.push_back(FormatDescription(affix, flags, map));
    } else if (affix[0] == '@') {
      affix = affix.substr(1);
      AttributeMap map;
      map.emplace("value", begin->value);
      result.push_back(FormatDescription(affix, flags, map));
    } else {
      static re::Prog subst(R"(\{(\w+)\})");
      affix = subst.replace(affix, [&affixData, begin](re::Match const& m) {
        std::string key = m.group(1);
        std::string param = fmtstring("%d", begin->param);
        return affixData.raw[key][param].getString();
      });
      size_t dollar = affix.find('$');
      if (dollar != std::string::npos) {
        int index = atoi(affix.substr(dollar + 1).c_str());
        affix.resize(dollar);
        aggregate[affix][index ? fmtstring("value%d", index) : "value"] = begin->value;
      } else {
        if (!attributes.has(affix)) continue;
        affix = attributes[affix];
        AttributeMap map = defaultMap;
        map.emplace("value", begin->value);
        map.emplace("value1", begin->value);
        result.push_back(FormatDescription(affix, flags, map));
      }
    }
  }
  for (auto& am : aggregate) {
    if (!attributes.has(am.first)) continue;
    result.push_back(FormatDescription(attributes[am.first], flags, am.second));
  }
  return result;
}

std::vector<std::string> GameAffixes::format(std::vector<AttributeSpecifier> const& attrs, FormatFlags flags) {
  return format(attrs.data(), attrs.data() + attrs.size(), flags);
}
