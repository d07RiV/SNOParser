#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Physics::Type) {
  SnoHeader x00_Header;
  int x0C;
  int x10;
  float x14;
  float x18;
  float x1C;
  float x20;
  float x24;
  int16 x28;
  uint8 x2A_;
  uint8 x2B_;
  float x2C;
  float x30_Acceleration;
  float x34_Acceleration;
  float x38_Velocity;
  float x3C_Acceleration;
  float x40_Acceleration;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x14, x18, x1C);
    dumpval(x20, x24, x28, x2C, x30_Acceleration, x34_Acceleration);
    dumpval(x38_Velocity, x3C_Acceleration, x40_Acceleration);
  }
};

#pragma pack(pop)
