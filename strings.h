#pragma once
#include "common.h"

class DictionaryRef {
public:
  DictionaryRef()
    : dict_(nullptr)
  {}

  operator bool() const {
    return dict_ != nullptr;
  }

  bool has(istring const& name) {
    return dict_ && dict_->find(name) != dict_->end();
  }
  std::string const& operator[](istring const& name) const {
    if (!dict_) return nil_;
    auto it = dict_->find(name);
    return (it == dict_->end() ? nil_ : it->second);
  }
  std::string const& operator[](char const* name) const {
    return (*this)[istring(name)];
  }
  std::string const& getfmt(char const* fmt, ...) const;
private:
  friend class Strings;
  Dictionary const* dict_;
  static std::string nil_;
  DictionaryRef(Dictionary const* dict)
    : dict_(dict)
  {}
};

class Strings {
public:
  static DictionaryRef list(istring const& name);
  static std::string const& get(istring const& dict, istring const& name);
  static bool has(istring const& dict, istring const& name);
private:
  Map<Dictionary> strings_;
  Dictionary* get(istring const& dict);
  static Strings& instance();
};
