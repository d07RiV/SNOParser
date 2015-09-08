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

Dictionary* Strings::get(istring const& dict, SnoLoader* theloader) {
  Key key(dict, theloader);
  auto it = strings_.find(key);
  if (it != strings_.end()) return &it->second;
  if (!theloader) theloader = loader;
  if (!theloader) theloader = SnoLoader::default;
  SnoFile<StringList> list(dict, theloader);
  if (!list) return nullptr;
  Dictionary& res = strings_[key];
  for (auto& item : list->x10_StringTableEntries) {
    res.emplace(item.x00_Text.text(), item.x10_Text.text());
  }
  return &res;
}
DictionaryRef Strings::list(istring const& name, SnoLoader* loader) {
  return instance().get(name, loader);
}
std::string const& Strings::get(istring const& dict, istring const& name) {
  Dictionary* res = instance().get(dict, nullptr);
  if (res == nullptr) return DictionaryRef::nil_;
  auto it = res->find(name);
  return (it == res->end() ? DictionaryRef::nil_ : it->second);
}
bool Strings::has(istring const& dict, istring const& name) {
  Dictionary* res = instance().get(dict, SnoLoader::default);
  return res != nullptr && res->find(name) != res->end();
}

Strings& Strings::instance() {
  static Strings inst_;
  return inst_;
}
