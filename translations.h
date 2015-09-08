// translations.h
//
// SkillTips::dump() - generate skill tooltips JS files for all classes (for d3planner)

#pragma once
#include "common.h"
#include "json.h"
#include "parser.h"

std::string canonize(std::string const& str);
std::string untag(std::string const& str, bool capital = false);

struct SkillTips {
  std::string charClass;
  json::Value skills;
  json::Value passives;
  json::Value skillMap;
  json::Value passiveMap;
  json::Value categoryMap;

  void generate(std::string const& cls, uint32 kitId, json::Value* fix = nullptr);
  void read(std::string const& cls);
  void write();
  static std::string format(std::string const& power);
  static void dump();
};

void simBase(std::string const& name);
