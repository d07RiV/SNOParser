#include "itemlib.h"

ItemLibrary& ItemLibrary::instance() {
  static ItemLibrary inst;
  return inst;
}
ItemLibrary::ItemLibrary() {
  for (auto& name : Logger::Loop(SnoLoader::List<GameBalance>(), "Parsing items")) {
    files_.emplace_back(name);
    auto& file = files_.back();
    if (!file->x028_Items.size()) {
      files_.pop_back();
      continue;
    }
    for (auto& item : file->x028_Items) {
      items_[item.x000_Text] = &item;
    }
  }
}
