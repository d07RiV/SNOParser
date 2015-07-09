#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(PhysMesh::Type) {
  declstruct(dmFloat3) {
    float x00;
    float x04;
    float x08;
    void dumpfunc() {
      dumpval(x00, x04, x08);
    }
  };

  declstruct(dmFloat4) {
    float x00;
    float x04;
    float x08;
    float x0C;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C);
    }
  };

  declstruct(dmMeshNode) {
    int16 x00;
    uint8 x01_;
    int16 x02;
    uint8 x03_;
    int16 x04;
    uint8 x05_;
    int16 x06;
    uint8 x07_;
    int16 x08;
    uint8 x09_;
    int16 x0A;
    uint8 x0B_;
    int x0C;
    void dumpfunc() {
      dumpval(x00, x02, x04, x06, x08, x0A);
      dumpval(x0C);
    }
  };

  declstruct(dmMeshTriangle) {
    int x00;
    int x04;
    int x08;
    int x0C;
    int x10;
    int x14;
    int16 x18;
    uint8 x19_;
    int16 x1A;
    uint8 x1B_;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
      dumpval(x18, x1A);
    }
  };

  declstruct(CollisionMesh) {
    Array<dmMeshNode> x00_dmMeshNodes = x60_SerializeData;
    Array<dmFloat4> x08_dmFloat4s = x50_SerializeData;
    Array<dmMeshTriangle> x10_dmMeshTriangles = x58_SerializeData;
    dmFloat3 x18_dmFloat3;
    dmFloat3 x24_dmFloat3;
    dmFloat3 x30_dmFloat3;
    int x3C;
    int x40;
    int x44;
    int x48;
    uint32 x4C_;
    SerializeData x50_SerializeData;
    SerializeData x58_SerializeData;
    SerializeData x60_SerializeData;
    int x68;
    int x6C;
    void dumpfunc() {
      dumpval(x00_dmMeshNodes, x08_dmFloat4s, x10_dmMeshTriangles, x18_dmFloat3, x24_dmFloat3, x30_dmFloat3);
      dumpval(x3C, x40, x44, x48, x68, x6C);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  int x10;
  SerializeData x14_SerializeData;
  uint32 x1C_;
  Array<CollisionMesh> x20_CollisionMeshs = x14_SerializeData;
  int x28;
  uint32 x2C_;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x20_CollisionMeshs, x28);
  }
};

#pragma pack(pop)
