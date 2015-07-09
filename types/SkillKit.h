#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(SkillKit::Type) {
  declstruct(TraitEntry) {
    Sno<Power> x00_PowerSno;
    int x04;
    int x08;
    int x0C;
    void dumpfunc() {
      dumpval(x00_PowerSno, x04, x08, x0C);
    }
  };

  declstruct(ActiveSkillEntry) {
    Sno<Power> x00_PowerSno;
    int x04;
    int x08;
    int x0C;
    int x10;
    int x14;
    int x18;
    int x1C;
    int x20;
    int x24;
    int x28;
    int x2C_int[5];
    void dumpfunc() {
      dumpval(x00_PowerSno, x04, x08, x0C, x10, x14);
      dumpval(x18, x1C, x20, x24, x28, x2C_int);
    }
  };

  SnoHeader x00_Header;
  uint32 x0C_;
  Array<TraitEntry> x10_TraitEntries = x18_SerializeData;
  SerializeData x18_SerializeData;
  Array<ActiveSkillEntry> x20_ActiveSkillEntries = x28_SerializeData;
  SerializeData x28_SerializeData;
  Array<int> x30_int = x38_SerializeData;
  SerializeData x38_SerializeData;
  void dumpfunc() {
    dumpval(x00_Header, x10_TraitEntries, x20_ActiveSkillEntries, x30_int);
  }
};

#pragma pack(pop)
