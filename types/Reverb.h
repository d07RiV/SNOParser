#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Reverb::Type) {
  declstruct(ReverbParams) {
    int x00;
    float x04;
    float x08;
    int x0C;
    int x10;
    int x14;
    float x18;
    float x1C;
    float x20;
    int x24;
    float x28;
    int x2C;
    float x30;
    float x34;
    float x38;
    float x3C;
    float x40;
    float x44;
    float x48;
    float x4C;
    float x50;
    int x54;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
      dumpval(x18, x1C, x20, x24, x28, x2C);
      dumpval(x30, x34, x38, x3C, x40, x44);
      dumpval(x48, x4C, x50, x54);
    }
  };

  SnoHeader x00_Header;
  ReverbParams x0C_ReverbParams;
  int x64;
  void dumpfunc() {
    dumpval(x00_Header, x0C_ReverbParams, x64);
  }
};

#pragma pack(pop)
