#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Vibrations::Type) {
  declstruct(FloatNode) {
    float x00;
    float x04;
    float x08;
    void dumpfunc() {
      dumpval(x00, x04, x08);
    }
  };

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

  declstruct(FloatPath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<FloatNode> x20_FloatNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_FloatNodes);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  float x10_Time;
  float x14_Time;
  float x18;
  float x1C;
  float x20;
  uint32 x24_;
  FloatPath x28_FloatPath;
  FloatPath x58_FloatPath;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10_Time, x14_Time, x18, x1C);
    dumpval(x20, x28_FloatPath, x58_FloatPath);
  }
};

#pragma pack(pop)
