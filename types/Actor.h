#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Actor::Type) {
  declstruct(DT_VECTOR3D) {
    float x00_X;
    float x04_Y;
    float x08_Z;
    void dumpfunc() {
      dumpval(x00_X, x04_Y, x08_Z);
    }
  };

  declstruct(AxialCylinder) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    float x0C;
    float x10;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C, x10);
    }
  };

  declstruct(Sphere) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    float x0C;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C);
    }
  };

  declstruct(AABB) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    DT_VECTOR3D x0C_DT_VECTOR3D;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C_DT_VECTOR3D);
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

  declstruct(LookLink) {
    char x00_Text[64];
    void dumpfunc() {
      dumpval(x00_Text);
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
    float x18C_Time;
    DT_RGBACOLOR x190_DT_RGBACOLOR;
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

  declstruct(WeightedLook) {
    LookLink x00_LookLink;
    int x40;
    void dumpfunc() {
      dumpval(x00_LookLink, x40);
    }
  };

  declstruct(ActorCollisionFlags) {
    int x00;
    int x04;
    int x08;
    int x0C;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C);
    }
  };

  declstruct(ActorCollisionData) {
    ActorCollisionFlags x00_ActorCollisionFlags;
    int x10;
    AxialCylinder x14_AxialCylinder;
    AABB x28_AABB;
    float x40;
    void dumpfunc() {
      dumpval(x00_ActorCollisionFlags, x10, x14_AxialCylinder, x28_AABB, x40);
    }
  };

  declstruct(InventoryImages) {
    int x00;
    int x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  SnoHeader x000_Header;
  int x00C;
  int x010_Enum; // Invalid = 0, Monster = 1, Gizmo = 2, Client Effect = 3, Server Prop = 4, Environment = 5, Critter = 6, Player = 7, Item = 8, Axe Symbol = 9, Projectile = 10, Custom Brain = 11
  Sno<Appearance> x014_AppearanceSno;
  Sno<PhysMesh> x018_PhysMeshSno;
  AxialCylinder x01C_AxialCylinder;
  Sphere x030_Sphere;
  AABB x040_AABB;
  SerializeData x058_SerializeData;
  Array<int> x060_TagMap = x058_SerializeData; // type: 0x00010000
  Sno<AnimSet> x068_AnimSetSno;
  Sno<Monster> x06C_MonsterSno;
  SerializeData x070_SerializeData;
  int x078;
  uint32 x07C_;
  Array<MsgTriggeredEvent> x080_MsgTriggeredEvents = x070_SerializeData;
  DT_VECTOR3D x088_DT_VECTOR3D;
  WeightedLook x094_WeightedLooks[8];
  Sno<Physics> x2B4_PhysicsSno;
  int x2B8;
  int x2BC;
  float x2C0;
  float x2C4;
  float x2C8;
  ActorCollisionData x2CC_ActorCollisionData;
  InventoryImages x310_InventoryImages[6];
  int x340;
  uint32 x344_;
  Text x348_Text = x350_SerializeData;
  SerializeData x350_SerializeData;
  Text x358_Text = x360_SerializeData;
  SerializeData x360_SerializeData;
  void dumpfunc() {
    dumpval(x000_Header, x00C, x010_Enum, x014_AppearanceSno, x018_PhysMeshSno, x01C_AxialCylinder);
    dumpval(x030_Sphere, x040_AABB, x060_TagMap, x068_AnimSetSno, x06C_MonsterSno, x078);
    dumpval(x080_MsgTriggeredEvents, x088_DT_VECTOR3D, x094_WeightedLooks, x2B4_PhysicsSno, x2B8, x2BC);
    dumpval(x2C0, x2C4, x2C8, x2CC_ActorCollisionData, x310_InventoryImages, x340);
    dumpval(x348_Text, x358_Text);
  }
};

#pragma pack(pop)
