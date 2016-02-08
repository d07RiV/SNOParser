#include "powertag.h"
#include "types/Power.h"
#include "types/GameBalance.h"
#include "description.h"
#include <iostream>
#include <set>

PowerTags::PowerTags(SnoLoader* loader) {
  json::Value tags;
  json::parse(File("tags.txt"), tags);
  for (auto& kv : tags.getMap()) {
    uint32 id = atoi(kv.first.c_str());
    std::string name = kv.second["name"].getString();
    tags_[name] = id;
    reverse_[id] = name;
    rawnames_[id] = kv.second["tag"].getString();
  }
  for (auto& gmb : loader->all<GameBalance>()) {
    for (auto& pow : gmb->x198_PowerFormulaTable) {
      auto& tbl = tables_[pow.x000_Text];
      for (int i = 0; i < 76; ++i) {
        tbl.entries[i] = (&pow.x400)[i];
      }
    }
  }
  static uint32 mapOffsets[] = {
    0x008, 0x018, 0x028, 0x050, 0x058, 0x060, 0x068, 0x090, 0x098, 0x0A0, 0x0A8
  };
  for (auto& pow : loader->all<Power>()) {
    PowerTag& power = powers_[pow.name()];
    raw_[pow->x000_Header.id] = &power;
    power.name_ = pow.name();
    uint8* base = reinterpret_cast<uint8*>(&pow->x050_PowerDef);
    using PowerTags = Power::Type::PowerTags;
    for (uint32 offset : mapOffsets) {
      uint32 const* data = reinterpret_cast<PowerTags*>(base + offset)->data();
      uint32 count = *data++;
      while (count--) {
        uint32 type = *data++;
        uint32 id = *data++;
        if (type != 4) {
          power.formulas_.emplace(id, (int)*data++);
        } else {
          data += 5;
          uint32 len_name = *data++;
          data += 1;
          uint32 len_data = *data++;
          char const* text = (char*)data;
          data += (len_name + 3) / 4;
          len_data = (len_data + 3) / 4;
          power.formulas_.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(id),
            std::forward_as_tuple(data, data + len_data, text)
          );
          data += len_data;
        }
      }
    }
    for (size_t sf = 0; sf < pow->x438_ScriptFormulaDetails.size(); ++sf) {
      auto it = power.formulas_.find(PowerTag::sfid(sf));
      if (it != power.formulas_.end()) {
        it->second.comment = pow->x438_ScriptFormulaDetails[sf].x000_Text;
      }
    }
  }
}

PowerTags& PowerTags::instance(SnoLoader* loader) {
  static PowerTags inst_(loader);
  return inst_;
}

AttributeValue PowerTag::_get(uint32 id, ScriptFormula& sf, AttributeMap const& attr) {
  if (sf.state == sDone) return sf.value;
  if (sf.state == sCurrent) {
    throw Exception("recursive formula in PowerTag.%s.\"%s\"", name_.c_str(), PowerTags::instance().reverse_[id].c_str());
  }
  sf.state = sCurrent;
  AttributeValue value = ExecFormula(sf.formula, attr, this);
  sf.state = sNone;
  return value;
}
Dictionary PowerTag::formulas() {
  auto& tags = PowerTags::instance().reverse_;
  Dictionary values;
  for (auto& kv : formulas_) {
    values.emplace(tags[kv.first], kv.second.state == sDone ? fmtstring("%d", kv.second.value) : kv.second.text);
  }
  return values;
}

AttributeValue PowerTag::operator[](istring const& formula) {
  auto& tags = PowerTags::instance().tags_;
  auto it = tags.find(formula);
  return (it == tags.end() ? 0 : get(it->second));
}
AttributeValue PowerTag::get(istring const& formula, AttributeMap const& attr) {
  auto& tags = PowerTags::instance().tags_;
  auto it = tags.find(formula);
  return (it == tags.end() ? 0 : _get(it->second, attr));
}
uint32 PowerTag::getint(istring const& formula) {
  auto& tags = PowerTags::instance().tags_;
  auto it = tags.find(formula);
  if (it == tags.end()) return 0;
  auto it2 = formulas_.find(it->second);
  if (it2 == formulas_.end()) return 0;
  return (it2->second.state == sDone ? it2->second.value : 0);
}

std::string PowerTag::comment(istring const& formula) {
  auto& tags = PowerTags::instance().tags_;
  auto it = tags.find(formula);
  if (it == tags.end()) return 0;
  auto it2 = formulas_.find(it->second);
  return (it2 == formulas_.end() ? "" : it2->second.comment);
}

json::Value PowerTag::dump() const {
  auto& rawnames = PowerTags::instance().rawnames_;
  json::Value dst;
  for (auto& kv : formulas_) {
    auto& cur = dst[rawnames[kv.first]];
    if (kv.second.state == sDone) {
      cur = (uint32) kv.second.value;
    } else {
      cur.append(kv.second.text);
      std::string fmt;
      for (uint32 val : kv.second.formula) {
        if (!fmt.empty()) fmt.push_back(',');
        fmt.append(fmtstring("%08x", val));
      }
      cur.append(fmt);
      if (!kv.second.comment.empty()) {
        cur.append(kv.second.comment);
      }
    }
  }
  return dst;
}

json::Value PowerTags::dump() {
  auto& tags = instance().raw_;
  json::Value dst;
  for (auto& kv : tags) {
    auto& cur = dst[kv.second->name()];
    cur["id"] = kv.first;
    cur["tags"] = kv.second->dump();
  }
  return dst;
}
