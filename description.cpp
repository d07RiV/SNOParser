#include "description.h"
#include "common.h"
#include "regexp.h"
#include "powertag.h"
#include <stack>
#include <vector>
#include <algorithm>

int FormulaParser::next() {
  if (pos >= descr.size()) return tEnd;
  if (descr[pos] != '{') {
    chr = descr[pos++];
    return tChar;
  }
  ++pos;
  tag.clear();
  while (pos < descr.size() && descr[pos] != '}') {
    tag.push_back(descr[pos++]);
  }
  if (pos < descr.size()) {
    ++pos;
  }
  return tTag;
}
int FormulaParser::fnext() {
  while (pos < descr.size() && isspace((unsigned char)descr[pos])) ++pos;
  if (pos >= descr.size()) return tEnd;
  if (descr[pos] == '{') {
    ++pos;
    tag.clear();
    while (pos < descr.size() && descr[pos] != '}') {
      tag.push_back(descr[pos++]);
    }
    if (pos < descr.size()) {
      ++pos;
    }
    return tTag;
  }
  if (descr[pos] >= '0' && descr[pos] <= '9') {
    val = 0;
    std::string num;
    while (pos < descr.size() && descr[pos] >= '0' && descr[pos] <= '9') {
      num.push_back(descr[pos++]);
    }
    if (pos < descr.size() && descr[pos] == '.') {
      num.push_back(descr[pos++]);
      while (pos < descr.size() && descr[pos] >= '0' && descr[pos] <= '9') {
        num.push_back(descr[pos++]);
      }
    }
    val = atof(num.c_str());
    return tNum;
  }
  if (isalpha((unsigned char) descr[pos]) || descr[pos] == '_') {
    tag.clear();
    while (pos < descr.size() && (isalnum((unsigned char)descr[pos]) || descr[pos] == '_')) {
      tag.push_back(descr[pos++]);
    }
    return tName;
  }
  if (descr[pos] == '"') {
    tag.clear();
    while (pos < descr.size() && (tag.empty() || descr[pos] != '"')) {
      tag.push_back(descr[pos++]);
    }
    if (pos < descr.size()) {
      tag.push_back(descr[pos++]);
    }
    return tName;
  }
  chr = descr[pos++];
  return tChar;
}

Dictionary FormulaParser::pretags;
bool FormulaParser::preloaded = false;

void FormulaParser::addcolor(char const* color, char const* val) {
  if (!val) val = color;
  pretags.emplace(fmtstring("c_%s", color), fmtstring("<span class=\"d3-color-%s\">", val));
  pretags.emplace(fmtstring("/c_%s", color), "</span>");
}
FormulaParser::FormulaParser(std::string const& descr, bool html, AttributeMap const& values, PowerTag* context)
  : descr(descr)
  , html(html)
  , values(values)
  , pos(0)
  , context(context)
{
  if (!preloaded) {
    preloaded = true;
    addcolor("blue");
    addcolor("magic", "blue");
    addcolor("gray");
    addcolor("gold");
    addcolor("green");
    addcolor("orange");
    addcolor("purple");
    addcolor("red");
    addcolor("white");
    addcolor("yellow");
    pretags.emplace("/c", "</span>");
    pretags.emplace("icon:bullet", "<span class=\"tooltip-icon-bullet\"></span>");
  }
}

std::string FormulaParser::Value::format() {
  if (text.c_str()) {
    return text;
  } else if (min == max) {
    return fmtstring(plus ? "%+.*lf" : "%.*lf", digits, max);
  } else {
    return fmtstring(plus ? "%+.*lf-%.*lf" : "%.*lf-%.*lf", digits, min, digits, max);
  }
}

static const char funcTable = -128;
static const char funcMin = -127;
static const char funcMax = -126;
struct OpInfo {
  char sym;
  int lprio, rprio;
  int lval, rval;
};
static OpInfo opInfos[] = {
  { 0, 0, 0, 0, 0 },
  { '{', 0, 0, 0, 1 },
  { '}', 0, 0, 0, 0 },
  { '(', 13, 0, 1, 1 },
  { ')', 0, 13, 0, 0 },
  { '+', 7, 7, 0, 1 },
  { '-', 7, 7, 0, 1 },
  { '*', 8, 8, 0, 1 },
  { '/', 8, 8, 0, 1 },
  { '~', 10, 9, 1, 1 },
  { '<', 6, 6, 0, 1 },
  { '>', 6, 6, 0, 1 },
  { '?', 5, 5, 0, 1 },
  { ':', 4, 4, 0, 1 },
  { ',', 1, 1, 0, 1 },

  { funcTable, 13, 0, 1, 1 },
  { funcMin, 13, 0, 1, 1 },
  { funcMax, 13, 0, 1, 1 },
};
static OpInfo const& opInfo(char sym) {
  for (auto& op : opInfos) {
    if (op.sym == sym) return op;
  }
  return opInfos[0];
}

struct EvalStack {
  std::stack<AttributeValue> vals;
  std::stack<char> ops;
  int needval = 1;
  void exec(char chr);
};

static inline AttributeValue binop(AttributeValue const& lhs, AttributeValue const& rhs, char op) {
  switch (op) {
  case '+': return lhs + rhs;
  case '-': return lhs - rhs;
  case '*': return lhs * rhs;
  case '/': return lhs / rhs;
  case '>': return (lhs.max > rhs.max ? 1.0 : 0.0);
  case '<': return (lhs.max < rhs.max ? 1.0 : 0.0);
  case funcMin: return AttributeValue(std::min(lhs.min, rhs.min), std::min(lhs.max, rhs.max));
  case funcMax: return AttributeValue(std::max(lhs.min, rhs.min), std::max(lhs.max, rhs.max));
  case funcTable:
    if (lhs.table == nullptr) {
      return 0;
    } else {
      int imin = static_cast<int>(rhs.min);
      int imax = static_cast<int>(rhs.max);
      if (imin < 0 || imax > 75) return 0;
      return AttributeValue(std::min(lhs.table[imin], lhs.table[imax]),
        std::max(lhs.table[imin], lhs.table[imax]));
    }
    break;
  default: return AttributeValue();
  }
}
void EvalStack::exec(char chr) {
  AttributeValue a, b, c;
  switch (chr) {
  case '+':
  case '-':
  case '*':
  case '/':
  case '>':
  case '<':
  case funcMin:
  case funcMax:
  case funcTable:
    if (vals.size() < 2) return;
    b = vals.top(); vals.pop();
    a = vals.top(); vals.pop();
    vals.push(binop(a, b, chr));
    break;
  case '~':
    if (vals.size() < 1) return;
    a = vals.top(); vals.pop();
    vals.emplace(-a.max, -a.min);
    break;
  case ':':
    if (vals.size() < 3) return;
    c = vals.top(); vals.pop();
    b = vals.top(); vals.pop();
    a = vals.top(); vals.pop();
    vals.push(a.max ? b : c);
    break;
  }
}

static char getFunction(std::string const& name) {
  std::string id = strlower(name);
  if (id == "table") return funcTable;
  if (id == "min") return funcMin;
  if (id == "max") return funcMax;
  return '(';
}

void FormulaParser::endeval() {
  while (int type = fnext()) {
    if (type == tChar && chr == ']') break;
  }
}
FormulaParser::Value FormulaParser::eval() {
  int digits = 0;
  bool plus = false;

  EvalStack stack;
  stack.ops.push('{');
  while (int type = fnext()) {
    if (type == tChar && chr == ']') break;
    if (type == tChar && chr == '|') {
      type = fnext();
      if (type == tNum) {
        digits = static_cast<int>(val);
      } else if (type == tChar && chr == '+') {
        plus = true;
      }
      fnext();
    } else if (type == tName) {
      std::vector<std::string> parts;
      bool isfunc = false;
      while (type == tName) {
        parts.push_back(tag);
        size_t prev = pos;
        if ((type = fnext()) != tChar || chr != '.') {
          if (type == tChar && chr == '(') {
            isfunc = true;
          } else {
            pos = prev;
          }
          break;
        }
        type = fnext();
      }
      std::string result = "";
      for (auto& part : parts) {
        if (!result.empty()) result.push_back('.');
        result.append(part);
      }
      if (isfunc) {
        char funcId = getFunction(result);
        OpInfo const& op = opInfo(funcId);
        while (stack.ops.size() && opInfo(stack.ops.top()).rprio >= op.lprio) {
          stack.exec(stack.ops.top());
          stack.ops.pop();
        }
        stack.ops.push(funcId);
        stack.needval = op.rval;
      } else if (!stack.ops.empty() && stack.ops.top() == funcTable) {
        stack.vals.push(AttributeValue(PowerTags::table(result)));
      } else {
        static re::Prog sfid("sf_(\\d+)", -1, re::Prog::CaseInsensitive);
        static re::Prog sftag(R"/(powertag.(\w+)."(.*)")/", -1, re::Prog::CaseInsensitive);
        //static re::Prog lookup(R"(table.(\w+).(\w+))", -1, re::Prog::CaseInsensitive);
        std::vector<std::string> match;
        if (context && sfid.match(result, &match)) {
          stack.vals.emplace(context->get(atoi(match[1].c_str()), values));
        } else if (sftag.match(result, &match)) {
          stack.vals.emplace(PowerTags::get(match[1], match[2], values));
        } else {
          auto it = values.find(result);
          stack.vals.push(it == values.end() ? 0.0 : it->second);
        }
      }
      stack.needval = 0;
    } else if (type == tTag) {
      auto it = values.find(tag);
      if (it != values.end()) {
        prevtag = it->second;
      } else if (context) {
        prevtag = context->get(tag, values);
      } else {
        prevtag = 0.0;
      }
      stack.vals.push(prevtag);
      stack.needval = 0;
    } else if (type == tNum) {
      stack.vals.push(val);
      stack.needval = 0;
    } else if (type == tChar) {
      if (chr == '-' && stack.needval) chr = '~';
      OpInfo const& op = opInfo(chr);
      while (stack.ops.size() && opInfo(stack.ops.top()).rprio >= op.lprio) {
        char top = stack.ops.top();
        stack.exec(top);
        stack.ops.pop();
        if (opInfo(top).rprio == 0) break;
      }
      if (chr != ')') stack.ops.push(chr);
      stack.needval = op.rval;
    }
  }
  OpInfo const& op = opInfo('}');
  while (stack.ops.size() && opInfo(stack.ops.top()).rprio >= op.lprio) {
    stack.exec(stack.ops.top());
    stack.ops.pop();
  }
  AttributeValue res;
  if (stack.vals.size()) res = stack.vals.top();
  return Value(res, digits, plus);
}

std::string FormulaParser::parse() {
  std::string result;
  int newlines = 0;
  while (int type = next()) {
    if (html && newlines && (type != tChar || chr != '\n')) {
      if (newlines == 1) {
        result.append("<br/>");
      } else {
        result.append("</p><p>");
      }
      newlines = 0;
    }
    switch (type) {
    case tChar:
      if (chr == '[') {
        result.append(eval().format());
      } else if (chr == '|') {
        next(); // 4
        std::string lhs, rhs;
        while (next() == tChar && chr != ':') {
          lhs.push_back(chr);
        }
        while (next() == tChar && chr != ';') {
          rhs.push_back(chr);
        }
        if (prevtag.min != 1 || prevtag.max != 1) {
          result.append(rhs);
        } else {
          result.append(lhs);
        }
      } else if (chr == '\n' && html) {
        ++newlines;
      } else {
        result.push_back(chr);
      }
      break;
    case tTag:
      if (tag.substr(0, 2) == "/c" || tag.substr(0, 2) == "c:" || pretags.find(tag) != pretags.end()) {
        if (html) {
          if (tag.substr(0, 2) == "/c") {
            result.append("</span>");
          } else if (tag.substr(0, 2) == "c:") {
            result.append(fmtstring("<span style=\"color: #%s\">", tag.substr(4).c_str()));
          } else {
            result.append(pretags[tag]);
          }
        }
      } else {
        auto it = values.find(tag);
        if (it != values.end()) {
          prevtag = it->second;
        } else if (context) {
          prevtag = context->get(tag, values);
        } else {
          prevtag = 0.0;
        }
        result.append(Value(prevtag).format());
      }
      break;
    }
  }
  return result;
}

std::string FormatDescription(std::string const& descr, bool html, AttributeMap const& values, PowerTag* context) {
  static re::Prog bracketer(R"(([0-9]+{\.[0-9]+}?)-([0-9]+{\.[0-9]+}?)-([0-9]+{\.[0-9]+}?)-([0-9]+{\.[0-9]+}?))");
  static re::Prog dasher("([0-9)])-([0-9(])");
  FormulaParser parser(descr, html, values, context);
  std::string result = parser.parse();
  result = bracketer.replace(result, "(\\1-\\2)-(\\3-\\4)");
  if (html) {
    result = dasher.replace(result, "\\1&#x2013;\\2");
  }
  return result;
}

static inline AttributeValue getval(AttributeMap const& values, istring const& name) {
  auto it = values.find(name);
  return (it == values.end() ? 0 : it->second);
}
static char const* elemNames[] = {
  "Physical", "Fire", "Lightning", "Cold", "Poison", "Arcane", "Holy"
};

AttributeValue ExecFormula(uint32 const* begin, uint32 const* end, AttributeMap const& values, PowerTag* context) {
  std::stack<AttributeValue> stack;
  AttributeValue a, b, c;
  while (begin < end) {
    switch (*begin++) {
    case 0:
      return (stack.empty() ? 0 : stack.top());
    case 1:
      switch (*begin++) {
      case 0:
        b = stack.top(); stack.pop();
        a = stack.top(); stack.pop();
        stack.emplace(std::min(a.min, b.min), std::min(a.max, b.max));
        break;
      case 1:
        b = stack.top(); stack.pop();
        a = stack.top(); stack.pop();
        stack.emplace(std::max(a.min, b.min), std::max(a.max, b.max));
        break;
      case 2:
        c = stack.top(); stack.pop();
        b = stack.top(); stack.pop();
        a = stack.top(); stack.pop();
        stack.emplace(std::min(std::max(a.min, b.min), b.min), std::min(std::max(a.max, b.max), c.max));
        break;
      case 3:
      case 4:
      case 10:
        b = stack.top(); stack.pop();
        a = stack.top(); stack.pop();
        stack.emplace(std::min(a.min, b.min), std::max(a.max, b.max));
        break;
      case 11:
        b = stack.top(); stack.pop();
        a = stack.top(); stack.pop();
        stack.push(binop(a, b, funcTable));
        break;
      default:
        return 0;
      }
      break;
    case 5: {
      uint32 x = *begin++;
      uint32 y = *begin++;
      uint32 z = *begin++;
      uint32 w = *begin++;
      switch (x) {
      case 0:
        switch (y) {
        case 0: stack.push(getval(values, "Defense")); break;
        case 10: stack.push(getval(values, "Strength")); break;
        case 57: stack.push(getval(values, "Strength")); break;
        case 102: stack.push(getval(values, "Skill_Total")); break;
        case 187: stack.push(getval(values, "Casting_Speed_Total")); break;
        case 198: stack.push(getval(values, "Attacks_Per_Second_Total")); break;
        case 209: stack.push(getval(values, fmtstring("Damage_Delta#%s", elemNames[z]))); break;
        case 211: stack.push(getval(values, fmtstring("Damage_Min#%s", elemNames[z]))); break;
        case 677: stack.push(getval(values, "Rune_A")); break;
        case 678: stack.push(getval(values, "Rune_B")); break;
        case 679: stack.push(getval(values, "Rune_C")); break;
        case 680: stack.push(getval(values, "Rune_D")); break;
        case 681: stack.push(getval(values, "Rune_E")); break;
        case 745: stack.push(getval(values, "Buff_Icon_Count0")); break;
        case 746: stack.push(getval(values, "Buff_Icon_Count1")); break;
        case 747: stack.push(getval(values, "Buff_Icon_Count2")); break;
        case 748: stack.push(getval(values, "Buff_Icon_Count3")); break;
        case 1056: stack.push(getval(values, "Resource_Gain_Bonus_Percent")); break;
        case 1377: stack.push(getval(values, "Effective_Level")); break;
        default: stack.push(0);
        }
        break;
      case 1: stack.push(getval(values, "sLevel")); break;
      case 2: stack.push(getval(values, "iLevel")); break;
      case 3: stack.push(getval(values, "mLevel")); break;
      case 6: stack.push(getval(values, "mDamageMin#Physical")); break;
      case 7: stack.push(getval(values, "mDamageDelta#Physical")); break;
      case 8: stack.push(getval(values, "mDamageMin#Fire")); break;
      case 9: stack.push(getval(values, "mDamageDelta#Fire")); break;
      case 10: stack.push(getval(values, "mDamageMin#Lightning")); break;
      case 11: stack.push(getval(values, "mDamageDelta#Lightning")); break;
      case 12: stack.push(getval(values, "mDamageMin#Cold")); break;
      case 13: stack.push(getval(values, "mDamageDelta#Cold")); break;
      case 14: stack.push(getval(values, "mDamageMin#Poison")); break;
      case 15: stack.push(getval(values, "mDamageDelta#Poison")); break;
      case 16: stack.push(getval(values, "mDamageMin#Arcane")); break;
      case 17: stack.push(getval(values, "mDamageDelta#Arcane")); break;
      case 18: stack.push(getval(values, "mDamageMin#Holy")); break;
      case 19: stack.push(getval(values, "mDamageDelta#Holy")); break;
      case 22: stack.push(PowerTags::getraw(y, z, values)); break;
        // 23...86 = SF_0...SF_63
      case 87: stack.emplace(PowerTags::table("DmgTier1")); break;
      case 88: stack.emplace(PowerTags::table("DmgTier2")); break;
      case 89: stack.emplace(PowerTags::table("DmgTier3")); break;
      case 90: stack.emplace(PowerTags::table("DmgTier4")); break;
      case 91: stack.emplace(PowerTags::table("DmgTier5")); break;
      case 92: stack.emplace(PowerTags::table("DmgTier6")); break;
      case 93: stack.emplace(PowerTags::table("DmgTier7")); break;
      case 94: stack.emplace(PowerTags::table("Healing")); break;
      case 95: stack.emplace(PowerTags::table("WDCost")); break;
      case 96: stack.emplace(PowerTags::table("RuneDamageBonus")); break;
      case 97: stack.emplace(PowerTags::table("PvPAvgPrimaryStat")); break;
      case 98: stack.emplace(PowerTags::table("PvPAvgVitality")); break;
      case 99: stack.emplace(PowerTags::table("PvPAvgArmor")); break;
      case 100: stack.emplace(PowerTags::table("PvPAvgWeaponDPS")); break;
      case 101: stack.push(getval(values, "mHealthMin")); break;
      case 102: stack.emplace(PowerTags::table("LegendaryProcDmg")); break;
      case 103: stack.emplace(PowerTags::table("OffhandDmgMinMin")); break;
      case 104: stack.emplace(PowerTags::table("OffhandDmgMinMax")); break;
      case 105: stack.emplace(PowerTags::table("OffhandDmgDeltaMin")); break;
      case 106: stack.emplace(PowerTags::table("OffhandDmgDeltaMax")); break;
      case 107: stack.emplace(PowerTags::table("ShieldBlockMinMin")); break;
      case 108: stack.emplace(PowerTags::table("ShieldBlockMinMax")); break;
      case 109: stack.emplace(PowerTags::table("ShieldBlockDeltaMin")); break;
      case 110: stack.emplace(PowerTags::table("ShieldBlockDeltaMax")); break;
      case 111: stack.push(getval(values, "mAreaDamageMin")); break;
      case 112: stack.push(getval(values, "mAreaDamageMax")); break;
      case 113: stack.push(getval(values, "bTraitActive")); break;
      default:
        if (x >= 23 && x <= 86) {
          stack.push(context ? context->get(x - 23, values) : 0); break;
        } else {
          stack.push(0);
        }
      }
      break;
    }
    case 6:
      stack.push(*(float*)begin++);
      break;
    case 7:
      b = stack.top(); stack.pop();
      a = stack.top(); stack.pop();
      stack.push(a.max < b.max ? 1 : 0);
      break;
    case 8:
      b = stack.top(); stack.pop();
      a = stack.top(); stack.pop();
      stack.push(a.max > b.max ? 1 : 0);
      break;
    case 11:
      b = stack.top(); stack.pop();
      a = stack.top(); stack.pop();
      stack.push(a + b);
      break;
    case 12:
      b = stack.top(); stack.pop();
      a = stack.top(); stack.pop();
      stack.push(a - b);
      break;
    case 13:
      b = stack.top(); stack.pop();
      a = stack.top(); stack.pop();
      stack.push(a * b);
      break;
    case 14:
      b = stack.top(); stack.pop();
      a = stack.top(); stack.pop();
      stack.push(a / b);
      break;
    case 16:
      a = stack.top(); stack.pop();
      stack.emplace(-a.max, -a.min);
      break;
    case 17:
      c = stack.top(); stack.pop();
      b = stack.top(); stack.pop();
      a = stack.top(); stack.pop();
      stack.push(a.max ? b : c);
      break;
    default:
      return 0;
    }
  }
  return 0;
}
