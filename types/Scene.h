#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Scene::Type) {
  declstruct(DT_VECTOR3D) {
    float x00_X;
    float x04_Y;
    float x08_Z;
    void dumpfunc() {
      dumpval(x00_X, x04_Y, x08_Z);
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

  declstruct(NavMeshSquare) {
    float x00;
    int x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  declstruct(NavMeshDef) {
    int x00;
    int x04;
    int x08;
    int x0C;
    float x10;
    SerializeData x14_SerializeData;
    int x1C;
    Array<NavMeshSquare> x20_NavMeshSquares = x14_SerializeData;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x1C);
      dumpval(x20_NavMeshSquares);
    }
  };

  declstruct(LookLink) {
    char x00_Text[64];
    void dumpfunc() {
      dumpval(x00_Text);
    }
  };

  declstruct(ConstraintLink) {
    char x00_Text[64];
    void dumpfunc() {
      dumpval(x00_Text);
    }
  };

  declstruct(DT_RGBACOLOR) {
    uint8 x00;
    uint8 x01;
    uint8 x02;
    uint8 x03;
    void dumpfunc() {
      dumpval(x00, x01, x02, x03);
    }
  };

  declstruct(HardpointLink) {
    char x00_Text[64];
    int x40;
    void dumpfunc() {
      dumpval(x00_Text, x40);
    }
  };

  declstruct(TriggerConditions) {
    int x00_Percent;
    float x04_Time;
    float x08_Time;
    float x0C_Time;
    float x10_Time;
    float x14_Impulse;
    float x18_Impulse;
    int x1C;
    int x20;
    void dumpfunc() {
      dumpval(x00_Percent, x04_Time, x08_Time, x0C_Time, x10_Time, x14_Impulse);
      dumpval(x18_Impulse, x1C, x20);
    }
  };

  declstruct(TriggerEvent) {
    int x000;
    TriggerConditions x004_TriggerConditions;
    int x028;
    SNOName x02C_SNOName;
    int x034;
    int x038;
    int x03C;
    int x040;
    HardpointLink x044_HardpointLinks[2];
    LookLink x0CC_LookLink;
    ConstraintLink x10C_ConstraintLink;
    int x14C;
    float x150;
    int x154;
    int x158;
    int x15C;
    int x160;
    int x164;
    float x168;
    float x16C;
    int x170;
    float x174;
    int x178;
    float x17C_Velocity;
    int x180;
    float x184_Time;
    DT_RGBACOLOR x188_DT_RGBACOLOR;
    uint8 x18B_;
    float x18C_Time;
    DT_RGBACOLOR x190_DT_RGBACOLOR;
    uint8 x193_;
    float x194_Time;
    void dumpfunc() {
      dumpval(x000, x004_TriggerConditions, x028, x02C_SNOName, x034, x038);
      dumpval(x03C, x040, x044_HardpointLinks, x0CC_LookLink, x10C_ConstraintLink, x14C);
      dumpval(x150, x154, x158, x15C, x160, x164);
      dumpval(x168, x16C, x170, x174, x178, x17C_Velocity);
      dumpval(x180, x184_Time, x188_DT_RGBACOLOR, x18C_Time, x190_DT_RGBACOLOR, x194_Time);
    }
  };

  declstruct(MsgTriggeredEvent) {
    int x000;
    TriggerEvent x004_TriggerEvent;
    void dumpfunc() {
      dumpval(x000, x004_TriggerEvent);
    }
  };

  declstruct(DT_IVECTOR2D) {
    int x00;
    int x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  declstruct(NavCell) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    uint32 x08_;
    DT_VECTOR3D x0C_DT_VECTOR3D;
    uint32 x14_;
    int16 x18;
    uint8 x19_;
    int16 x1A;
    uint8 x1B_;
    int x1C;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C_DT_VECTOR3D, x18, x1A, x1C);
    }
  };

  declstruct(NavCellBorderData) {
    int16 x00;
    uint8 x01_;
    int16 x02;
    uint8 x03_;
    void dumpfunc() {
      dumpval(x00, x02);
    }
  };

  declstruct(NavCellLookup) {
    int16 x00;
    uint8 x01_;
    int16 x02;
    uint8 x03_;
    void dumpfunc() {
      dumpval(x00, x02);
    }
  };

  declstruct(NavGridSquare) {
    int16 x00;
    uint8 x01_;
    int16 x02;
    uint8 x03_;
    int16 x04;
    uint8 x05_;
    void dumpfunc() {
      dumpval(x00, x02, x04);
    }
  };

  declstruct(NavZoneDefinition) {
    int x00;
    uint32 x04_;
    Array<NavCell> x08_NavCells = x10_SerializeData;
    SerializeData x10_SerializeData;
    int x18;
    uint32 x1C_;
    Array<NavCellLookup> x20_NavCellLookups = x28_SerializeData;
    SerializeData x28_SerializeData;
    float x30;
    float x34;
    int x38;
    DT_IVECTOR2D x3C_DT_IVECTOR2D;
    uint32 x40_;
    uint32 x44_;
    Array<NavGridSquare> x48_NavGridSquares = x50_SerializeData;
    SerializeData x50_SerializeData;
    int x58;
    uint32 x5C_;
    Array<NavCellLookup> x60_NavCellLookups = x68_SerializeData;
    SerializeData x68_SerializeData;
    int x70;
    uint32 x74_;
    Array<NavCellBorderData> x78_NavCellBorderDatas = x80_SerializeData;
    SerializeData x80_SerializeData;
    void dumpfunc() {
      dumpval(x00, x08_NavCells, x18, x20_NavCellLookups, x30, x34);
      dumpval(x38, x3C_DT_IVECTOR2D, x48_NavGridSquares, x58, x60_NavCellLookups, x70);
      dumpval(x78_NavCellBorderDatas);
    }
  };

  SnoHeader x000_Header;
  int x00C;
  AABB x010_AABB;
  AABB x028_AABB;
  NavMeshDef x040_NavMeshDef;
  SerializeData x068_SerializeData;
  Array<int> x070_Snos = x068_SerializeData;
  uint32 x078_;
  uint32 x07C_;
  uint32 x080_;
  uint32 x084_;
  uint32 x088_;
  uint32 x08C_;
  uint32 x090_;
  uint32 x094_;
  uint32 x098_;
  uint32 x09C_;
  uint32 x0A0_;
  uint32 x0A4_;
  SerializeData x0A8_SerializeData;
  Array<int> x0B0_Snos = x0A8_SerializeData;
  uint32 x0B8_;
  uint32 x0BC_;
  uint32 x0C0_;
  uint32 x0C4_;
  uint32 x0C8_;
  uint32 x0CC_;
  uint32 x0D0_;
  uint32 x0D4_;
  uint32 x0D8_;
  uint32 x0DC_;
  uint32 x0E0_;
  uint32 x0E4_;
  SerializeData x0E8_SerializeData;
  Array<int> x0F0_Snos = x0E8_SerializeData;
  uint32 x0F8_;
  uint32 x0FC_;
  uint32 x100_;
  uint32 x104_;
  uint32 x108_;
  uint32 x10C_;
  uint32 x110_;
  uint32 x114_;
  uint32 x118_;
  uint32 x11C_;
  uint32 x120_;
  uint32 x124_;
  LookLink x128_LookLink;
  SerializeData x168_SerializeData;
  int x170;
  uint32 x174_;
  Array<MsgTriggeredEvent> x178_MsgTriggeredEvents = x168_SerializeData;
  NavZoneDefinition x180_NavZoneDefinition;
  Sno<Appearance> x208_AppearanceSno;
  Sno<PhysMesh> x20C_PhysMeshSno;
  void dumpfunc() {
    dumpval(x000_Header, x00C, x010_AABB, x028_AABB, x040_NavMeshDef, x070_Snos);
    dumpval(x0B0_Snos, x0F0_Snos, x128_LookLink, x170, x178_MsgTriggeredEvents, x180_NavZoneDefinition);
    dumpval(x208_AppearanceSno, x20C_PhysMeshSno);
  }
};

#pragma pack(pop)
