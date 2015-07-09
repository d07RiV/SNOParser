#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(FlagSet::Type) {
  declstruct(FlagLookup) {
    int x000;
    char x004_Text[128];
    char x084_Text[256];
    void dumpfunc() {
      dumpval(x000, x004_Text, x084_Text);
    }
  };

  SnoHeader x00_Header;
  uint32 x0C_;
  Array<FlagLookup> x10_FlagLookups = x18_SerializeData;
  SerializeData x18_SerializeData;
  void dumpfunc() {
    dumpval(x00_Header, x10_FlagLookups);
  }
};

#pragma pack(pop)
