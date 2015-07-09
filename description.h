#pragma once
#include <string>
#include <map>
#include <vector>
#include "common.h"

struct AttributeValue {
  double min, max;
  double const* table;
  std::string text;
  AttributeValue(double min, double max)
    : min(min), max(max), table(nullptr)
  {}
  AttributeValue(double val)
    : min(val), max(val), table(nullptr)
  {}
  AttributeValue(std::string const& text)
    : min(0), max(0), table(nullptr), text(text)
  {}
  explicit AttributeValue(double const* ptr)
    : min(0), max(0), table(ptr)
  {}
  AttributeValue()
    : min(0), max(0), table(nullptr)
  {}
  void include(double x) {
    if (x < min) min = x;
    if (x > max) max = x;
  }

  friend AttributeValue operator + (AttributeValue const& lhs, AttributeValue const& rhs) {
    return AttributeValue(lhs.min + rhs.min, lhs.max + rhs.max);
  }
  friend AttributeValue operator - (AttributeValue const& lhs, AttributeValue const& rhs) {
    return AttributeValue(lhs.min - rhs.max, lhs.max - rhs.min);
  }
  friend AttributeValue operator * (AttributeValue const& lhs, AttributeValue const& rhs) {
    if (lhs.min > 0 && rhs.min > 0) {
      return AttributeValue(lhs.min * rhs.min, lhs.max * rhs.max);
    }
    AttributeValue res(lhs.min * rhs.min);
    res.include(lhs.min * rhs.max);
    res.include(lhs.max * rhs.min);
    res.include(lhs.max * rhs.max);
    if (lhs.min <= 0 && lhs.max >= 0) res.include(0);
    if (rhs.min <= 0 && rhs.max >= 0) res.include(0);
    return res;
  }
  friend AttributeValue operator / (AttributeValue const& lhs, AttributeValue const& rhs) {
    if (lhs.min > 0 && rhs.min > 0) {
      return AttributeValue(lhs.min / rhs.max, lhs.max / rhs.min);
    }
    AttributeValue res(lhs.min / rhs.min);
    res.include(lhs.min / rhs.max);
    res.include(lhs.max / rhs.min);
    res.include(lhs.max / rhs.max);
    if (lhs.min <= 0 && lhs.max >= 0) res.include(0);
    return res;
  }

  AttributeValue& operator+=(AttributeValue const& rhs) {
    return *this = *this + rhs;
  }
  AttributeValue& operator-=(AttributeValue const& rhs) {
    return *this = *this - rhs;
  }
  AttributeValue& operator*=(AttributeValue const& rhs) {
    return *this = *this * rhs;
  }
  AttributeValue& operator/=(AttributeValue const& rhs) {
    return *this = *this / rhs;
  }
};

typedef Map<AttributeValue> AttributeMap;
class PowerTag;

AttributeValue ExecFormula(uint32 const* begin, uint32 const* end, AttributeMap const& values = {}, PowerTag* context = nullptr);
inline AttributeValue ExecFormula(std::vector<uint32> const& formula, AttributeMap const& values = {}, PowerTag* context = nullptr) {
  return ExecFormula(formula.data(), formula.data() + formula.size(), values, context);
}

struct FormulaParser {
  enum { tEnd, tChar, tTag, tNum, tName };
  std::string descr;
  bool html;
  size_t pos;
  AttributeMap values;
  PowerTag* context;
  AttributeValue prevtag;
  static Dictionary pretags;
  static bool preloaded;
  static void addcolor(char const* color, char const* val = nullptr);
  char chr;
  std::string tag;
  double val;
  int next();
  int fnext();
  void endeval();
public:
  FormulaParser(std::string const& descr, bool html, AttributeMap const& values = {}, PowerTag* context = nullptr);

  struct Value : public AttributeValue {
    int digits = 0;
    bool plus = false;

    Value() {}
    Value(AttributeValue const& val, int digits = 0, bool plus = false)
      : AttributeValue(val)
      , digits(digits)
      , plus(plus)
    {}
    std::string format();
  };

  Value eval();
  std::string parse();
};

std::string FormatDescription(std::string const& descr, bool html, AttributeMap const& values = {}, PowerTag* context = nullptr);
