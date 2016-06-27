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
  std::map<uint32, std::string> const& get() const {
    return map_;
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
  static void loadTOC(uint8 const* toc);

  template<class T>
  static const SnoMap& get() {
    auto it = instance_.map_.find(T::index);
    if (it == instance_.map_.end()) {
      SnoMap& map = instance_.map_[T::index];
      if (!map.load(T::type())) {
        for (auto& name : Logger::Loop(SnoLoader::default->list<T>(),
            fmtstring("Mapping %s", T::type()).c_str())) {
          map.parse(SnoLoader::default->load<T>(name), name);
        }
        map.save(T::type());
      }
      return map;
    } else {
      return it->second;
    }
  }
  static const SnoMap& gameBalance();
  static void clear();
private:
  static SnoManager instance_;
  std::map<uint32, SnoMap> map_;
};
