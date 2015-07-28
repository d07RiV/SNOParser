// translations.h
//
// SkillTips::dump() - generate skill tooltips JS files for all classes (for d3planner)

#pragma once
#include "common.h"
#include "json.h"

std::string canonize(std::string const& str);

struct SkillTips {
  std::string charClass;
  json::Value skills;
  json::Value passives;

  void generate(std::string const& cls, uint32 kitId);
  void read(std::string const& cls);
  void write();
  static std::string format(std::string const& power);
  static void dump();
};

void simBase(std::string const& name);
