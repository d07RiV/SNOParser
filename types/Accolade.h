#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Accolade::Type) {
  SnoHeader x00_Header;
  int x0C;
  void dumpfunc() {
    dumpval(x00_Header, x0C);
  }
};

#pragma pack(pop)
