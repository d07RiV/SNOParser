#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Observer::Type) {
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
  float x10_Angle;
  float x14;
  float x18;
  float x1C;
  float x20_Angle;
  float x24_Angle;
  float x28;
  DT_VECTOR3D x2C_DT_VECTOR3D;
  uint32 x34_;
  DT_VECTOR3D x38_DT_VECTOR3D;
  uint32 x40_;
  float x44;
  float x48;
  float x4C;
  float x50;
  float x54;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10_Angle, x14, x18, x1C);
    dumpval(x20_Angle, x24_Angle, x28, x2C_DT_VECTOR3D, x38_DT_VECTOR3D, x44);
    dumpval(x48, x4C, x50, x54);
  }
};

#pragma pack(pop)
