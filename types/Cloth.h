#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Cloth::Type) {
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
  float x14;
  float x18;
  float x1C;
  float x20;
  float x24;
  float x28;
  float x2C;
  float x30_Acceleration;
  float x34;
  float x38;
  float x3C;
  int x40_int[4];
  Sno<AmbientSound> x50_AmbientSoundSno;
  int x54;
  DT_VECTOR3D x58_DT_VECTOR3D;
  uint32 x60_;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x14, x18, x1C);
    dumpval(x20, x24, x28, x2C, x30_Acceleration, x34);
    dumpval(x38, x3C, x40_int, x50_AmbientSoundSno, x54, x58_DT_VECTOR3D);
  }
};

#pragma pack(pop)
