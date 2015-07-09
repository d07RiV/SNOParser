#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Account::Type) {
  SnoHeader x00_Header;
  SerializeData x0C_SerializeData;
  uint32 x14_;
  Array<uint8> x18_byte = x0C_SerializeData;
  void dumpfunc() {
    dumpval(x00_Header, x18_byte);
  }
};

#pragma pack(pop)
