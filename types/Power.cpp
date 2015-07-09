#include "Power.h"

void Power::Type::PowerTags::serialize(json::Visitor* visitor) {
  static std::map<int, std::string> powerTags;
  static bool loaded = false;
  if (!loaded) {
    json::Value tags;
    json::parse(File("tags.txt"), tags);
    for (auto& kv : tags.getMap()) {
      powerTags[atoi(kv.first.c_str())] = kv.second["tag"].getString();
    }
    loaded = true;
  }
  uint32 const* ptr = data();
  uint32 count = *ptr++;
  visitor->onOpenMap();
  while (count--) {
    uint32 type = *ptr++;
    uint32 id = *ptr++;
    visitor->onMapKey(powerTags[id]);
    if (type != 4) {
      visitor->onInteger(*ptr++);
    } else {
      ptr += 5;
      uint32 len_name = *ptr++;
      ptr += 1;
      uint32 len_data = *ptr++;
      visitor->onOpenArray();
      visitor->onString((char*) ptr);
      ptr += (len_name + 3) / 4;
      len_data = (len_data + 3) / 4;
      std::string fmt;
      while (len_data--) {
        if (!fmt.empty()) fmt.push_back(',');
        fmt += fmtstring("%08X", *ptr++);
      }
      visitor->onString(fmt);
      visitor->onCloseArray();
    }
  }
  visitor->onCloseMap();
}
