#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(MarkerSet::Type) {
  declstruct(MarkerLink) {
    char x000_Text[128];
    char x080_Text[128];
    void dumpfunc() {
      dumpval(x000_Text, x080_Text);
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

  declstruct(Quaternion) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    uint32 x08_;
    float x0C;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C);
    }
  };

  declstruct(PRTransform) {
    Quaternion x00_Quaternion;
    DT_VECTOR3D x10_DT_VECTOR3D;
    uint32 x18_;
    void dumpfunc() {
      dumpval(x00_Quaternion, x10_DT_VECTOR3D);
    }
  };

  declstruct(SNOName) {
    int x00_SnoGroup;
    int x04_SnoNameHandle;
    void dumpfunc() {
      dumpval(x00_SnoGroup, x04_SnoNameHandle);
    }
  };

  declstruct(Marker) {
    char x00_Text[128];
    int x80;
    PRTransform x84_PRTransform;
    SNOName xA0_SNOName;
    SerializeData xA8_SerializeData;
    Array<int> xB0_TagMap = xA8_SerializeData; // type: 0x00080000
    int xB8;
    SerializeData xBC_SerializeData;
    uint32 xC4_;
    Array<MarkerLink> xC8_MarkerLinks = xBC_SerializeData;
    void dumpfunc() {
      dumpval(x00_Text, x80, x84_PRTransform, xA0_SNOName, xB0_TagMap, xB8);
      dumpval(xC8_MarkerLinks);
    }
  };

  declstruct(DT_VECTOR2D) {
    float x00;
    float x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  declstruct(Circle) {
    DT_VECTOR2D x00_DT_VECTOR2D;
    uint32 x04_;
    float x08;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR2D, x08);
    }
  };

  declstruct(AABB) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    uint32 x08_;
    DT_VECTOR3D x0C_DT_VECTOR3D;
    uint32 x14_;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C_DT_VECTOR3D);
    }
  };

  SnoHeader x00_Header;
  SerializeData x0C_SerializeData;
  uint32 x14_;
  Array<Marker> x18_Markers = x0C_SerializeData;
  uint32 x20_;
  uint32 x24_;
  uint32 x28_;
  uint32 x2C_;
  uint32 x30_;
  uint32 x34_;
  uint32 x38_;
  uint32 x3C_;
  uint32 x40_;
  uint32 x44_;
  uint32 x48_;
  uint32 x4C_;
  SerializeData x50_SerializeData;
  Array<Circle> x58_Circles = x50_SerializeData;
  uint32 x60_;
  uint32 x64_;
  uint32 x68_;
  uint32 x6C_;
  uint32 x70_;
  uint32 x74_;
  uint32 x78_;
  uint32 x7C_;
  uint32 x80_;
  uint32 x84_;
  uint32 x88_;
  uint32 x8C_;
  AABB x90_AABB;
  int xA8;
  GameBalanceId xAC_LabelsGameBalanceId;
  int xB0;
  SerializeData xB4_SerializeData;
  uint32 xBC_;
  Array<int16> xC0_short = xB4_SerializeData;
  void dumpfunc() {
    dumpval(x00_Header, x18_Markers, x58_Circles, x90_AABB, xA8, xAC_LabelsGameBalanceId);
    dumpval(xB0, xC0_short);
  }
};

#pragma pack(pop)
