#pragma once
#include "types/GameBalance.h"
#include "types/Power.h"
#include "json.h"
#include <set>

void parseItem(GameBalance::Type::Item const& item, json::Value& to, bool html);
void parseSetBonus(GameBalance::Type::SetItemBonusTableEntry const& bonus, json::Value& to, bool html);
void parsePower(Power::Type const& power, json::Value& to, bool html);

void jsonCompare(json::Value& lhs, json::Value& rhs);

std::vector<std::string> mergeKeys(json::Value const& lhs, json::Value const& rhs, std::set<std::string> const& excl = {});
void diff(File& file, json::Value const& lhs, json::Value const& rhs, std::vector<std::string> const& order = {}, bool links = false);
void makehtml(File& file, json::Value const& val, std::vector<std::string> const& order = {}, bool links = false);
