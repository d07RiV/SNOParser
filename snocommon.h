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
    char const* name = T::name(id);
    if (name) {
      visitor->onIntegerEx(id, name);
    } else {
      visitor->onInteger(id);
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
