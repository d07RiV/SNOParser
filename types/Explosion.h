#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Explosion::Type) {
  declstruct(DT_VECTOR3D) {
    float x00_X;
    float x04_Y;
    float x08_Z;
    void dumpfunc() {
      dumpval(x00_X, x04_Y, x08_Z);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  int x10;
  DT_VECTOR3D x14_DT_VECTOR3D;
  uint32 x1C_;
  float x20_Impulse;
  float x24;
  float x28;
  float x2C;
  float x30;
  int x34;
  float x38;
  int x3C;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x14_DT_VECTOR3D, x20_Impulse, x24);
    dumpval(x28, x2C, x30, x34, x38, x3C);
  }
};

#pragma pack(pop)
