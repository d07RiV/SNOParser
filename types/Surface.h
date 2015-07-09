#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Surface::Type) {
  SnoHeader x00_Header;
  int x0C;
  Sno<Particle> x10_ParticleSno;
  int x14;
  SerializeData x18_SerializeData;
  Array<int> x20_TagMap = x18_SerializeData; // type: 0x00050000
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10_ParticleSno, x14, x20_TagMap);
  }
};

#pragma pack(pop)
