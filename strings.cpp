#include "strings.h"
#include "types/StringList.h"

std::string DictionaryRef::nil_;
std::string const& DictionaryRef::getfmt(char const* fmt, ...) const {
  va_list ap;
  va_start(ap, fmt);
  std::string key = varfmtstring(fmt, ap);
  va_end(ap);
  return (*this)[key];
}

Dictionary* Strings::get(istring const& dict) {
  auto it = strings_.find(dict);
  if (it != strings_.end()) return &it->second;
  SnoFile<StringList> list(dict);
  if (!list) return nullptr;
  Dictionary& res = strings_[dict];
  for (auto& item : list->x10_StringTableEntries) {
    res.emplace(item.x00_Text.text(), item.x10_Text.text());
  }
  return &res;
}
DictionaryRef Strings::list(istring const& name) {
  return instance().get(name);
}
std::string const& Strings::get(istring const& dict, istring const& name) {
  Dictionary* res = instance().get(dict);
  if (res == nullptr) return DictionaryRef::nil_;
  auto it = res->find(name);
  return (it == res->end() ? DictionaryRef::nil_ : it->second);
}
bool Strings::has(istring const& dict, istring const& name) {
  Dictionary* res = instance().get(dict);
  return res != nullptr && res->find(name) != res->end();
}

Strings& Strings::instance() {
  static Strings inst_;
  return inst_;
}
