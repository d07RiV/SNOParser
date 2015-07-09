#pragma once
#include "common.h"
#include "json.h"

std::string canonize(std::string const& str);

struct SkillTips {
  std::string charClass;
  json::Value skills;
  json::Value passives;

  void read(std::string const& cls);
  void write();
  void generate(std::string const& cls, uint32 kit);

  static void dump();
};
