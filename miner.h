// miner.h
//
// dump item/set/skill into json
// compare json files
//
// parseItem, parseSetBonus, parsePower - parse into json
//
// jsonCompare - compare two json values, only leave different entries
//
// diff, makehtml - generate diff/dump json as html, json values must have a certain structure:
//   id: {name1: value1, name2: value2, ...}, where values are plain numbers/strings, or arrays of numbers/strings
//   see uses in garbage.cpp for details
//
// fulldiff<T> - find *all* differences between two game versions (in files of type T)

#pragma once
#include "types/GameBalance.h"
#include "types/Power.h"
#include "json.h"
#include <set>

void parseItem(GameBalance::Type::Item const& item, json::Value& to, bool html);
void parseSetBonus(GameBalance::Type::SetItemBonusTableEntry const& bonus, json::Value& to, bool html);
void parsePower(Power::Type const& power, json::Value& to, bool html);

void jsonCompare(json::Value& lhs, json::Value& rhs, bool noArrays = true);

std::vector<std::string> mergeKeys(json::Value const& lhs, json::Value const& rhs, std::set<std::string> const& excl = {});
void diff(File& file, json::Value const& lhs, json::Value const& rhs, std::vector<std::string> const& order = {}, bool links = false);
void makehtml(File& file, json::Value const& val, std::vector<std::string> const& order = {}, bool links = false);

namespace MinerPrivate {
  typedef void(*ParseFunc)(SnoLoader&, std::string const&, json::Visitor*);
  typedef std::vector<istring>(*ListFunc)(SnoLoader&);
  template<class T>
  void parse(SnoLoader& loader, std::string const& name, json::Visitor* visitor) {
    File src = loader.load<T>(name);
    T::parse(src, visitor);
  }
  template<class T>
  std::vector<istring> list(SnoLoader& loader) {
    std::vector<istring> res;
    for (auto& name : loader.list<T>()) {
      res.push_back(name);
    }
    std::sort(res.begin(), res.end());
    res.resize(std::unique(res.begin(), res.end()) - res.begin());
    return res;
  }
  void fulldiff(SnoLoader& lhs, SnoLoader& rhs,
    ParseFunc func, ListFunc list, std::string const& type);
}

template<class T>
void fulldiff(SnoLoader& lhs, SnoLoader& rhs) {
  MinerPrivate::fulldiff(lhs, rhs, MinerPrivate::parse<T>, MinerPrivate::list<T>, T::type());
}
