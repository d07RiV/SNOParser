#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Shakes::Type) {
  declstruct(InterpolationScalar) {
    int x00;
    float x04;
    float x08;
    void dumpfunc() {
      dumpval(x00, x04, x08);
    }
  };

  declstruct(InterpolationPathHeader) {
    int x00;
    float x04;
    float x08;
    int x0C;
    InterpolationScalar x10_InterpolationScalar;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10_InterpolationScalar);
    }
  };

  declstruct(DT_VECTOR3D) {
    float x00_X;
    float x04_Y;
    float x08_Z;
    void dumpfunc() {
      dumpval(x00_X, x04_Y, x08_Z);
    }
  };

  declstruct(VectorNode) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    uint32 x08_;
    DT_VECTOR3D x0C_DT_VECTOR3D;
    uint32 x14_;
    float x18;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C_DT_VECTOR3D, x18);
    }
  };

  declstruct(VectorPath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<VectorNode> x20_VectorNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_VectorNodes);
    }
  };

  declstruct(AngleNode) {
    float x00_Angle;
    float x04_Angle;
    float x08;
    void dumpfunc() {
      dumpval(x00_Angle, x04_Angle, x08);
    }
  };

  declstruct(AnglePath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<AngleNode> x20_AngleNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_AngleNodes);
    }
  };

  declstruct(FloatNode) {
    float x00;
    float x04;
    float x08;
    void dumpfunc() {
      dumpval(x00, x04, x08);
    }
  };

  declstruct(FloatPath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<FloatNode> x20_FloatNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_FloatNodes);
    }
  };

  SnoHeader x000_Header;
  int x00C;
  float x010_Time;
  float x014_Time;
  float x018;
  float x01C;
  VectorPath x020_VectorPath;
  VectorPath x050_VectorPath;
  AnglePath x080_AnglePath;
  FloatPath x0B0_FloatPath;
  AnglePath x0E0_AnglePath;
  FloatPath x110_FloatPath;
  AnglePath x140_AnglePath;
  FloatPath x170_FloatPath;
  void dumpfunc() {
    dumpval(x000_Header, x00C, x010_Time, x014_Time, x018, x01C);
    dumpval(x020_VectorPath, x050_VectorPath, x080_AnglePath, x0B0_FloatPath, x0E0_AnglePath, x110_FloatPath);
    dumpval(x140_AnglePath, x170_FloatPath);
  }
};

#pragma pack(pop)
