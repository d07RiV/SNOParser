#pragma once
#include "common.h"
#include "description.h"
#include "types/GameBalance.h"
#include <set>

struct AttributeSpecifier;
struct AffixValue;

#ifdef PTR
static inline int fixAttrId(int id) {
  return id;
}
#else
static inline int fixAttrId(int id) {
  if (id >= 672) {
    return id + 5;
  } else if (id >= 165) {
    return id + 3;
  } else {
    return id;
  }
}
#endif

class GameAffixes {
public:
  static AffixValue const& getAffix(uint32 id);
  static std::vector<AffixValue> getGroup(uint32 id, uint32 itemType);
  static AttributeMap const& defaultMap() {
    return instance().defaultMap_;
  }

  static std::vector<std::string> format(AttributeSpecifier const* begin, AttributeSpecifier const* end, bool html = false);
  static std::vector<std::string> format(std::vector<AttributeSpecifier> const& attrs, bool html = false);
  static uint32 itemTypeParent(uint32 id) {
    return instance().itemTypeParent_[id];
  }
  static bool isSecondary(uint32 attr);

private:
  struct GameAffix;
  AttributeMap defaultMap_;
  std::map<uint32, GameAffix> affixes_;
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
    return basic_compare(lhs.value.max, rhs.value.max);
  }
  static bool less(AttributeSpecifier const& lhs, AttributeSpecifier const& rhs) {
    return compare(lhs, rhs) < 0;
  }
};

struct AffixValue {
  static const int MaxAttributes = 4;
  AttributeSpecifier attributes[MaxAttributes];

  std::vector<std::string> format(bool html = false) const {
    return GameAffixes::format(attributes, attributes + MaxAttributes, html);
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

