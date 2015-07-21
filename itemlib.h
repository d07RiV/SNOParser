#pragma once
#include "types/GameBalance.h"
#include <list>

class ItemLibrary {
public:
  static GameBalance::Type::Item* get(istring const& id) {
    auto& dir = instance().items_;
    auto it = dir.find(id);
    return (it == dir.end() ? nullptr : it->second);
  }
private:
  static ItemLibrary& instance();
  ItemLibrary();
  std::list<SnoFile<GameBalance>> files_;
  Map<GameBalance::Type::Item*> items_;
};
