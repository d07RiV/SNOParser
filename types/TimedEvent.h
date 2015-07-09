#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(TimedEvent::Type) {
  SnoHeader x00_Header;
  float x0C_Time;
  void dumpfunc() {
    dumpval(x00_Header, x0C_Time);
  }
};

#pragma pack(pop)
