#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(StringList::Type) {
  declstruct(StringTableEntry) {
    Text x00_Text = x08_SerializeData;
    SerializeData x08_SerializeData;
    Text x10_Text = x18_SerializeData;
    SerializeData x18_SerializeData;
    int x20;
    int x24;
    void dumpfunc() {
      dumpval(x00_Text, x10_Text, x20, x24);
    }
  };

  SnoHeader x00_Header;
  uint32 x0C_;
  Array<StringTableEntry> x10_StringTableEntries = x18_SerializeData;
  SerializeData x18_SerializeData;
  uint32 x20_;
  uint32 x24_;
  void dumpfunc() {
    dumpval(x00_Header, x10_StringTableEntries);
  }
};

#pragma pack(pop)

Dictionary readStrings(char const* name, SnoLoader* loader = SnoLoader::default);
