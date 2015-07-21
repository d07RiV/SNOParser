#include "common.h"
#include "parser.h"
#include "description.h"

class PowerTag {
  enum FormulaState { sNone, sCurrent, sDone };
  struct ScriptFormula {
    FormulaState state;
    int value;
    std::vector<uint32> formula;
    std::string text;
    std::string comment;
    ScriptFormula(int value)
      : state(sDone)
      , value(value)
    {}
    ScriptFormula(uint32 const* begin, uint32 const* end, char const* text = nullptr)
      : state(sNone)
      , formula(begin, end)
      , text(text ? text : "")
    {}
  };
  friend class PowerTags;
  std::string name_;
  std::map<uint32, ScriptFormula> formulas_;
  static uint32 sfid(int id) {
    return 0x41100 + (id % 10) * 0x10 + (id / 10) * 0x100;
  }
  AttributeValue _get(uint32 id, ScriptFormula& sf, AttributeMap const& attr);
  AttributeValue _get(uint32 id, AttributeMap const& attr) {
    auto it = formulas_.find(id);
    if (it == formulas_.end()) return 0;
    return _get(id, it->second, attr);
  }
public:
  AttributeValue operator[](istring const& formula);
  AttributeValue operator[](int id) {
    if (id < 0 || id > 63) return 0;
    return get(sfid(id), {});
  }
  AttributeValue get(istring const& formula, AttributeMap const& attr = {});
  uint32 getint(istring const& formula);
  AttributeValue get(int id, AttributeMap const& attr = {}) {
    if (id < 0 || id > 63) return 0;
    return _get(sfid(id), attr);
  }
  AttributeValue getraw(uint32 id, AttributeMap const& attr = {}) {
    return _get(id, attr);
  }
  std::string comment(istring const& formula);
  std::string comment(int id) {
    if (id < 0 || id > 63) return 0;
    auto it = formulas_.find(sfid(id));
    return (it == formulas_.end() ? "" : it->second.comment);
  }
  std::string const& name() const {
    return name_;
  }

  Dictionary formulas();
};

class PowerTags {
  PowerTags(SnoLoader* loader = SnoLoader::default);
public:
  static PowerTags& instance(SnoLoader* loader = SnoLoader::default);
  static PowerTag* get(istring const& name) {
    auto& pow = instance().powers_;
    auto it = pow.find(name);
    return (it == pow.end() ? nullptr : &it->second);
  }
  static AttributeValue get(istring const& name, int id, AttributeMap const& attr = {}) {
    return instance()[name].get(id, attr);
  }
  static AttributeValue get(istring const& name, istring const& formula, AttributeMap const& attr = {}) {
    return instance()[name].get(formula, attr);
  }
  static PowerTag* getraw(uint32 power_id) {
    auto& raw = instance().raw_;
    auto it = raw.find(power_id);
    return (it == raw.end() ? nullptr : it->second);
  }
  static AttributeValue getraw(uint32 power_id, uint32 formula_id, AttributeMap const& attr = {}) {
    auto& raw = instance().raw_;
    auto it = raw.find(power_id);
    return (it == raw.end() ? 0 : it->second->getraw(formula_id, attr));
  }
  static double const* table(istring const& name) {
    auto& inst = instance();
    auto it = inst.tables_.find(name);
    if (it == inst.tables_.end()) return nullptr;
    return it->second.entries;
  }

  PowerTag& operator[](istring const& name) {
    auto it = powers_.find(name);
    return (it == powers_.end() ? nil_ : it->second);
  }
private:
  friend class PowerTag;
  Map<PowerTag> powers_;
  std::map<uint32, PowerTag*> raw_;
  Map<uint32> tags_;
  std::map<uint32, std::string> reverse_;
  PowerTag nil_;
  struct PowerTable {
    double entries[76];
  };
  Map<PowerTable> tables_;
};
