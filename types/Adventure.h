#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Adventure::Type) {
  SnoHeader x00_Header;
  Sno<Actor> x0C_ActorSno;
  float x10;
  float x14_Angle;
  float x18_Angle;
  float x1C_Angle;
  Sno<MarkerSet> x20_MarkerSetSno;
  void dumpfunc() {
    dumpval(x00_Header, x0C_ActorSno, x10, x14_Angle, x18_Angle, x1C_Angle);
    dumpval(x20_MarkerSetSno);
  }
};

#pragma pack(pop)
