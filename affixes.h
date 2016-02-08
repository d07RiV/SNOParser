// affixes.h
//
// Manages item affixes (ones in GameBalance/1xx_AffixList.gam)
//   requires affixes.js in the work directory to function
//   https://github.com/d07RiV/SNOParser/blob/master/Work/affixes.js
//
// struct AttributeSpecifier - single attribute type/param/value
//   uint32 type - see affixes.js for list of types (note that type IDs change with game versions, use fixAttrId)
//   uint32 param
//   AttributeValue value
//
// struct AffixValue - affix bonus, consists of up to 4 attributes
//   AttributeSpecifier attributes[4]
//
// class GameAffixes (singleton, all methods are static)
//
// AffixValue getAffix(uint32 id)
//   returns value of one affix
// std::vector<AffixValue> getGroup(uint32 id, uint32 itemType)
//   returns the list of *different* affixes in the group (i.e. if there are many +str affixes,
//   it will only return the highest one)
//   filters attributes by itemType
// std::vector<std::string> format(AttributeSpecifier const* begin, AttributeSpecifier const* end, bool html = false)
//   format a list of attribute specifiers (will combine attributes like +min and +delta damage into one +X-Y damage)
// std::vector<std::string> format(std::vector<AttributeSpecifier> const& attrs, bool html = false)
//   same as above
// uint32 itemTypeParent(uint32 id)
//   returns parent of the item type (i.e. CrusaderShield -> Shield -> GenericOffHand -> Offhand)
// bool isSecondary(uint32 attr)
//   checks if an attribute type is secondary

#pragma once
#include "common.h"
#include "description.h"
#include "types/GameBalance.h"
#include <set>

struct AttributeSpecifier;
struct AffixValue;

int fixAttrId(int id, bool reverse = false);

class GameAffixes {
public:
  static AffixValue const& getAffix(uint32 id, bool recipe = false);
  static std::vector<AffixValue> getGroup(uint32 id, uint32 itemType);
  static AttributeMap const& defaultMap() {
    return instance().defaultMap_;
  }

  static std::vector<std::string> format(AttributeSpecifier const* begin, AttributeSpecifier const* end, FormatFlags flags = FormatNone);
  static std::vector<std::string> format(std::vector<AttributeSpecifier> const& attrs, FormatFlags flags = FormatNone);
  static uint32 itemTypeParent(uint32 id) {
    return instance().itemTypeParent_[id];
  }
  static bool isSecondary(uint32 attr);

private:
  struct GameAffix;
  AttributeMap defaultMap_;
  std::map<uint32, GameAffix> affixes_;
  std::map<uint32, GameAffix> affixesRecipe_;
  std::map<uint32, std::vector<GameAffix*>> groups_;
  std::map<uint32, uint32> itemTypeParent_;
  struct AffixData {
    std::map<uint32, std::string> types;
    std::set<uint32> secondary;
    json::Value raw;
  } affixData_;
  GameAffixes();
  static GameAffixes& instance();
};

struct AttributeSpecifier {
  uint32 type;
  uint32 param;
  AttributeValue value;

  AttributeSpecifier()
    : type(-1)
    , param(-1)
  {}
  AttributeSpecifier(GameBalance::Type::AttributeSpecifier const& attr, AttributeMap const& params = GameAffixes::defaultMap());

  static int compare(AttributeSpecifier const& lhs, AttributeSpecifier const& rhs) {
    if (lhs.type != rhs.type) return basic_compare(lhs.type, rhs.type);
    if (lhs.param != rhs.param) return basic_compare(lhs.param, rhs.param);
    if (lhs.value.max != rhs.value.max) return basic_compare(lhs.value.max, rhs.value.max);
    return basic_compare(lhs.value.min, rhs.value.min);
  }
  static bool less(AttributeSpecifier const& lhs, AttributeSpecifier const& rhs) {
    return compare(lhs, rhs) < 0;
  }
};

struct AffixValue {
  static const int MaxAttributes = 4;
  AttributeSpecifier attributes[MaxAttributes];

  std::vector<std::string> format(FormatFlags flags = FormatNone) const {
    return GameAffixes::format(attributes, attributes + MaxAttributes, flags);
  }

  static int compare(AffixValue const& lhs, AffixValue const& rhs) {
    for (int i = 0; i < MaxAttributes; ++i) {
      int delta = AttributeSpecifier::compare(lhs.attributes[i], rhs.attributes[i]);
      if (delta) return delta;
    }
    return 0;
  }
  static bool less(AffixValue const& lhs, AffixValue const& rhs) {
    return compare(lhs, rhs) < 0;
  }
};

