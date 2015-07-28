#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Particle::Type) {
  declstruct(InterpolationScalar) {
    int x00;
    float x04;
    float x08;
    void dumpfunc() {
      dumpval(x00, x04, x08);
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

  declstruct(IntNode) {
    int x00;
    int x04;
    float x08;
    void dumpfunc() {
      dumpval(x00, x04, x08);
    }
  };

  declstruct(IntPath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<IntNode> x20_IntNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_IntNodes);
    }
  };

  declstruct(TimeNode) {
    float x00_Time;
    float x04_Time;
    float x08;
    void dumpfunc() {
      dumpval(x00_Time, x04_Time, x08);
    }
  };

  declstruct(TimePath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<TimeNode> x20_TimeNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_TimeNodes);
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

  declstruct(VelocityVector3D) {
    float x00_Velocity;
    float x04_Velocity;
    float x08_Velocity;
    void dumpfunc() {
      dumpval(x00_Velocity, x04_Velocity, x08_Velocity);
    }
  };

  declstruct(VelocityVectorNode) {
    VelocityVector3D x00_VelocityVector3D;
    VelocityVector3D x0C_VelocityVector3D;
    float x18;
    void dumpfunc() {
      dumpval(x00_VelocityVector3D, x0C_VelocityVector3D, x18);
    }
  };

  declstruct(VelocityVectorPath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<VelocityVectorNode> x20_VelocityVectorNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_VelocityVectorNodes);
    }
  };

  declstruct(VelocityNode) {
    float x00_Velocity;
    float x04_Velocity;
    float x08;
    void dumpfunc() {
      dumpval(x00_Velocity, x04_Velocity, x08);
    }
  };

  declstruct(VelocityPath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<VelocityNode> x20_VelocityNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_VelocityNodes);
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
    DT_VECTOR3D x0C_DT_VECTOR3D;
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

  declstruct(DT_RGBACOLORVALUE) {
    float x00;
    float x04;
    float x08;
    float x0C;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C);
    }
  };

  declstruct(Material) {
    DT_RGBACOLORVALUE x00_DT_RGBACOLORVALUE;
    DT_RGBACOLORVALUE x10_DT_RGBACOLORVALUE;
    DT_RGBACOLORVALUE x20_DT_RGBACOLORVALUE;
    DT_RGBACOLORVALUE x30_DT_RGBACOLORVALUE;
    float x40;
    int x44;
    void dumpfunc() {
      dumpval(x00_DT_RGBACOLORVALUE, x10_DT_RGBACOLORVALUE, x20_DT_RGBACOLORVALUE, x30_DT_RGBACOLORVALUE, x40, x44);
    }
  };

  declstruct(FrameAnim) {
    int x00;
    float x04_Velocity;
    float x08_Velocity;
    int x0C;
    int x10;
    void dumpfunc() {
      dumpval(x00, x04_Velocity, x08_Velocity, x0C, x10);
    }
  };

  declstruct(Matrix4x4) {
    float x00;
    float x04;
    float x08;
    float x0C;
    float x10;
    float x14;
    float x18;
    float x1C;
    float x20;
    float x24;
    float x28;
    float x2C;
    float x30;
    float x34;
    float x38;
    float x3C;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
      dumpval(x18, x1C, x20, x24, x28, x2C);
      dumpval(x30, x34, x38, x3C);
    }
  };

  declstruct(TexAnimParams) {
    int x00;
    Matrix4x4 x04_Matrix4x4;
    float x44;
    float x48_Velocity;
    float x4C_Velocity;
    float x50;
    float x54_Velocity;
    float x58_Velocity;
    float x5C_Angle;
    float x60_AngularVelocity;
    float x64_AngularVelocity;
    int x68;
    int x6C;
    int x70;
    int x74;
    FrameAnim x78_FrameAnim;
    int x8C;
    void dumpfunc() {
      dumpval(x00, x04_Matrix4x4, x44, x48_Velocity, x4C_Velocity, x50);
      dumpval(x54_Velocity, x58_Velocity, x5C_Angle, x60_AngularVelocity, x64_AngularVelocity, x68);
      dumpval(x6C, x70, x74, x78_FrameAnim, x8C);
    }
  };

  declstruct(MaterialTexture) {
    Sno<Textures> x00_TexturesSno;
    TexAnimParams x04_TexAnimParams;
    int x94;
    void dumpfunc() {
      dumpval(x00_TexturesSno, x04_TexAnimParams, x94);
    }
  };

  declstruct(MaterialTextureEntry) {
    int x00;
    int x04;
    MaterialTexture x08_MaterialTexture;
    void dumpfunc() {
      dumpval(x00, x04, x08_MaterialTexture);
    }
  };

  declstruct(UberMaterial) {
    Sno<ShaderMap> x00_ShaderMapSno;
    Material x04_Material;
    SerializeData x4C_SerializeData;
    uint32 x54_;
    Array<MaterialTextureEntry> x58_MaterialTextureEntries = x4C_SerializeData;
    uint32 x60_;
    uint32 x64_;
    void dumpfunc() {
      dumpval(x00_ShaderMapSno, x04_Material, x58_MaterialTextureEntries);
    }
  };

  declstruct(EmitterParams) {
    FloatPath x000_FloatPath;
    FloatPath x030_FloatPath;
    VectorPath x060_VectorPath;
    int x090;
    char x094_Text[128];
    uint32 x114_;
    void dumpfunc() {
      dumpval(x000_FloatPath, x030_FloatPath, x060_VectorPath, x090, x094_Text);
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

  declstruct(ColorNode) {
    DT_RGBACOLOR x00_DT_RGBACOLOR;
    DT_RGBACOLOR x04_DT_RGBACOLOR;
    float x08;
    void dumpfunc() {
      dumpval(x00_DT_RGBACOLOR, x04_DT_RGBACOLOR, x08);
    }
  };

  declstruct(ColorPath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<ColorNode> x20_ColorNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_ColorNodes);
    }
  };

  declstruct(AngularVelocityNode) {
    float x00_AngularVelocity;
    float x04_AngularVelocity;
    float x08;
    void dumpfunc() {
      dumpval(x00_AngularVelocity, x04_AngularVelocity, x08);
    }
  };

  declstruct(AngularVelocityPath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<AngularVelocityNode> x20_AngularVelocityNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_AngularVelocityNodes);
    }
  };

  declstruct(AccelVector3D) {
    float x00_Acceleration;
    float x04_Acceleration;
    float x08_Acceleration;
    void dumpfunc() {
      dumpval(x00_Acceleration, x04_Acceleration, x08_Acceleration);
    }
  };

  declstruct(AccelVectorNode) {
    AccelVector3D x00_AccelVector3D;
    AccelVector3D x0C_AccelVector3D;
    float x18;
    void dumpfunc() {
      dumpval(x00_AccelVector3D, x0C_AccelVector3D, x18);
    }
  };

  declstruct(AccelVectorPath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<AccelVectorNode> x20_AccelVectorNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_AccelVectorNodes);
    }
  };

  declstruct(ConstraintLink) {
    char x00_Text[64];
    void dumpfunc() {
      dumpval(x00_Text);
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

  SnoHeader x000_Header;
  int x00C;
  int x010;
  float x014_Time;
  float x018_Time;
  float x01C_Time;
  InterpolationScalar x020_InterpolationScalar;
  int x02C;
  FloatPath x030_FloatPath;
  IntPath x060_IntPath;
  FloatPath x090_FloatPath;
  TimePath x0C0_TimePath;
  FloatPath x0F0_FloatPath;
  AnglePath x120_AnglePath;
  VelocityVectorPath x150_VelocityVectorPath;
  VelocityVectorPath x180_VelocityVectorPath;
  VelocityPath x1B0_VelocityPath;
  VelocityPath x1E0_VelocityPath;
  VelocityPath x210_VelocityPath;
  VectorPath x240_VectorPath;
  VelocityPath x270_VelocityPath;
  float x2A0;
  uint32 x2A4_;
  UberMaterial x2A8_UberMaterial;
  Sno<Physics> x310_PhysicsSno;
  float x314_Mass;
  int x318;
  float x31C;
  float x320;
  float x324;
  float x328;
  float x32C;
  float x330;
  float x334;
  Sno<Actor> x338_ActorSno;
  uint32 x33C_;
  EmitterParams x340_EmitterParams;
  ColorPath x458_ColorPath;
  FloatPath x488_FloatPath;
  FloatPath x4B8_FloatPath;
  FloatPath x4E8_FloatPath;
  FloatPath x518_FloatPath;
  AnglePath x548_AnglePath;
  AngularVelocityPath x578_AngularVelocityPath;
  AngularVelocityPath x5A8_AngularVelocityPath;
  AnglePath x5D8_AnglePath;
  VectorPath x608_VectorPath;
  FloatPath x638_FloatPath;
  VelocityPath x668_VelocityPath;
  AngularVelocityPath x698_AngularVelocityPath;
  VectorPath x6C8_VectorPath;
  FloatPath x6F8_FloatPath;
  VelocityPath x728_VelocityPath;
  VectorPath x758_VectorPath;
  VelocityVectorPath x788_VelocityVectorPath;
  AccelVectorPath x7B8_AccelVectorPath;
  VectorPath x7E8_VectorPath;
  VelocityVectorPath x818_VelocityVectorPath;
  AccelVectorPath x848_AccelVectorPath;
  VelocityPath x878_VelocityPath;
  FloatPath x8A8_FloatPath;
  int x8D8;
  float x8DC;
  float x8E0;
  SerializeData x8E4_SerializeData;
  int x8EC;
  Array<MsgTriggeredEvent> x8F0_MsgTriggeredEvents = x8E4_SerializeData;
  void dumpfunc() {
    dumpval(x000_Header, x00C, x010, x014_Time, x018_Time, x01C_Time);
    dumpval(x020_InterpolationScalar, x02C, x030_FloatPath, x060_IntPath, x090_FloatPath, x0C0_TimePath);
    dumpval(x0F0_FloatPath, x120_AnglePath, x150_VelocityVectorPath, x180_VelocityVectorPath, x1B0_VelocityPath, x1E0_VelocityPath);
    dumpval(x210_VelocityPath, x240_VectorPath, x270_VelocityPath, x2A0, x2A8_UberMaterial, x310_PhysicsSno);
    dumpval(x314_Mass, x318, x31C, x320, x324, x328);
    dumpval(x32C, x330, x334, x338_ActorSno, x340_EmitterParams, x458_ColorPath);
    dumpval(x488_FloatPath, x4B8_FloatPath, x4E8_FloatPath, x518_FloatPath, x548_AnglePath, x578_AngularVelocityPath);
    dumpval(x5A8_AngularVelocityPath, x5D8_AnglePath, x608_VectorPath, x638_FloatPath, x668_VelocityPath, x698_AngularVelocityPath);
    dumpval(x6C8_VectorPath, x6F8_FloatPath, x728_VelocityPath, x758_VectorPath, x788_VelocityVectorPath, x7B8_AccelVectorPath);
    dumpval(x7E8_VectorPath, x818_VelocityVectorPath, x848_AccelVectorPath, x878_VelocityPath, x8A8_FloatPath, x8D8);
    dumpval(x8DC, x8E0, x8EC, x8F0_MsgTriggeredEvents);
  }
};

#pragma pack(pop)
