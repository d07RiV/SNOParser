#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(ShaderMap::Type) {
  SnoHeader x00_Header;
  int x0C;
  SerializeData x10_SerializeData;
  Array<int> x18_TagMap = x10_SerializeData; // type: 0x00030000
  void dumpfunc() {
    dumpval(x00_Header, x0C, x18_TagMap);
  }
};

#pragma pack(pop)
