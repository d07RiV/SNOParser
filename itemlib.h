// itemlib.h
//
// item library (access item data by text ID)
//
// GameBalance::Type::Item* ItemLibrary::get(istring const& id)
//   get item data

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
