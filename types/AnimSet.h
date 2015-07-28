#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(AnimSet::Type) {
  declstruct(AnimSetTagMap) {
    SerializeData x00_SerializeData;
    Array<int> x08_TagMap = x00_SerializeData; // type: 0x000C0000
    void dumpfunc() {
      dumpval(x08_TagMap);
    }
  };

  SnoHeader x000_Header;
  Sno<AnimSet> x00C_AnimSetSno;
  AnimSetTagMap x010_AnimSetTagMaps[29];
  void dumpfunc() {
    dumpval(x000_Header, x00C_AnimSetSno, x010_AnimSetTagMaps);
  }
};

#pragma pack(pop)
