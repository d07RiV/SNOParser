#pragma once

#include "serialize.h"

#pragma pack(push, 1)

declstruct(SnoHeader) {
  int id, lock, flags;
  void dumpfunc() {
    dumpval(id, lock, flags);
  }
};

struct TagValue {
  uint32 value;
  operator uint32() const {
    return value;
  }
  void serialize(json::Visitor* visitor) {
    uint8 exponent = (value >> 23) & 0xFF;
    if (exponent != 0 && exponent != 255) {
      visitor->onNumber(*(float*)&value);
    } else {
      visitor->onInteger(value);
    }
  }
};

template<class T>
struct Sno {
  uint32 id;
  operator uint32() const {
    return id;
  }
  std::string name() const {
    char const* name = T::name(id);
    return (name ? name : "");
  }
  void serialize(json::Visitor* visitor) {
    if (id == -1) {
      visitor->onNull();
      return;
    }
    char const* name = T::name(id);
    if (name) {
      visitor->onIntegerEx(id, name);
    } else {
      visitor->onInteger(id);
    }
  }
};

struct SNOName {
  uint32 group;
  uint32 id;
  char const* c_type() const;
  char const* c_name() const;
  std::string type() const {
    char const* type = c_type();
    return (type ? type : "");
  }
  std::string name() const {
    char const* name = c_name();
    return (name ? name : "");
  }
  void serialize(json::Visitor* visitor) {
    if (group == -1 || id == -1) {
      visitor->onNull();
    } else {
      char const* type = c_type();
      char const* name = c_name();
      visitor->onOpenMap();
      visitor->onMapKey("x00_SnoGroup");
      if (type) {
        visitor->onIntegerEx(group, type);
      } else {
        visitor->onInteger(group);
      }
      visitor->onMapKey("x04_SnoNameHandle");
      if (name) {
        visitor->onIntegerEx(id, name);
      } else {
        visitor->onInteger(id);
      }
      visitor->onCloseMap();
    }
  }
};

struct GameBalanceId {
  uint32 id;
  operator uint32() const {
    return id;
  }
  std::string name() const {
    char const* name = SnoManager::gameBalance()[id];
    return (name ? name : "");
  }
  void serialize(json::Visitor* visitor) {
    char const* name = SnoManager::gameBalance()[id];
    if (name) {
      visitor->onIntegerEx(id, name);
    } else {
      visitor->onInteger(id);
    }
  }
};

struct float8 {
  uint8 raw;
  operator uint8() const {
    return raw;
  }
  float value() const {
    return (float(raw) - 128.0f) / 127.0f;
  }
  void serialize(json::Visitor* visitor) {
    visitor->onNumber(value());
  }
};

struct float16 {
  sint16 raw;
  operator uint16() const {
    return raw;
  }
  float value() const {
    return float(raw) / 32767.0f;
  }
  void serialize(json::Visitor* visitor) {
    visitor->onNumber(value());
  }
};

#pragma pack(pop)
