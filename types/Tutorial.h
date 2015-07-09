#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Tutorial::Type) {
  declstruct(DT_IVECTOR2D) {
    int x00;
    int x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  int x10;
  float x14_Time;
  float x18_Time;
  int x1C;
  int x20;
  DT_IVECTOR2D x24_DT_IVECTOR2D;
  uint32 x28_;
  int x2C;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x14_Time, x18_Time, x1C);
    dumpval(x20, x24_DT_IVECTOR2D, x2C);
  }
};

#pragma pack(pop)
