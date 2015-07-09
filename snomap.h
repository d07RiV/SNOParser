#pragma once

#include "types.h"
#include "parser.h"
#include "logger.h"
#include <map>
#include <string>

class File;

class SnoMap {
public:
  char const* operator[](uint32 id) const {
    auto it = map_.find(id);
    return (it == map_.end() ? nullptr : it->second.c_str());
  }
private:
  friend class SnoManager;
  std::map<uint32, std::string> map_;
  void save(std::string const& type);
  bool load(std::string const& type);
  void parse(File& file, std::string const& name);
};

class SnoManager {
public:
  template<class T>
  static const SnoMap& get() {
    std::string type = T::type();
    auto it = instance_.map_.find(type);
    if (it == instance_.map_.end()) {
      SnoMap& map = instance_.map_[type];
      //if (!map.load(type)) {
      for (auto& name : Logger::Loop(SnoLoader::default->list<T>(),
          fmtstring("Mapping %s", T::type()).c_str())) {
        map.parse(SnoLoader::default->load<T>(name), name);
      }
      //  map.save(type);
      //}
      return map;
    } else {
      return it->second;
    }
  }
  static const SnoMap& gameBalance();
private:
  static SnoManager instance_;
  std::map<std::string, SnoMap> map_;
};
