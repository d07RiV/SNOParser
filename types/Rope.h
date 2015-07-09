#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Rope::Type) {
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
    uint32 x0C_;
    DT_RGBACOLORVALUE x10_DT_RGBACOLORVALUE;
    uint32 x1C_;
    DT_RGBACOLORVALUE x20_DT_RGBACOLORVALUE;
    uint32 x2C_;
    DT_RGBACOLORVALUE x30_DT_RGBACOLORVALUE;
    uint32 x3C_;
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
    uint8 x03_;
    DT_RGBACOLOR x04_DT_RGBACOLOR;
    uint8 x07_;
    float x08;
    void dumpfunc() {
      dumpval(x00_DT_RGBACOLOR, x04_DT_RGBACOLOR, x08);
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

  declstruct(ColorPath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<ColorNode> x20_ColorNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_ColorNodes);
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

  declstruct(SNOName) {
    int x00_SnoGroup;
    int x04_SnoNameHandle;
    void dumpfunc() {
      dumpval(x00_SnoGroup, x04_SnoNameHandle);
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

  SnoHeader x000_Header;
  int x00C;
  int x010;
  float x014;
  float x018_Mass;
  float x01C;
  float x020_Mass;
  float x024;
  float x028;
  uint32 x02C_;
  UberMaterial x030_UberMaterial;
  Sno<Physics> x098_PhysicsSno;
  float x09C_Time;
  float x0A0_Time;
  float x0A4_Time;
  float x0A8_Time;
  uint32 x0AC_;
  ColorPath x0B0_ColorPath;
  ColorPath x0E0_ColorPath;
  FloatPath x110_FloatPath;
  FloatPath x140_FloatPath;
  FloatPath x170_FloatPath;
  FloatPath x1A0_FloatPath;
  FloatPath x1D0_FloatPath;
  FloatPath x200_FloatPath;
  FloatPath x230_FloatPath;
  FloatPath x260_FloatPath;
  FloatPath x290_FloatPath;
  FloatPath x2C0_FloatPath;
  VectorPath x2F0_VectorPath;
  VelocityPath x320_VelocityPath;
  float x350;
  float x354;
  FloatPath x358_FloatPath;
  FloatPath x388_FloatPath;
  FloatPath x3B8_FloatPath;
  FloatPath x3E8_FloatPath;
  FloatPath x418_FloatPath;
  SerializeData x448_SerializeData;
  int x450;
  uint32 x454_;
  Array<MsgTriggeredEvent> x458_MsgTriggeredEvents = x448_SerializeData;
  void dumpfunc() {
    dumpval(x000_Header, x00C, x010, x014, x018_Mass, x01C);
    dumpval(x020_Mass, x024, x028, x030_UberMaterial, x098_PhysicsSno, x09C_Time);
    dumpval(x0A0_Time, x0A4_Time, x0A8_Time, x0B0_ColorPath, x0E0_ColorPath, x110_FloatPath);
    dumpval(x140_FloatPath, x170_FloatPath, x1A0_FloatPath, x1D0_FloatPath, x200_FloatPath, x230_FloatPath);
    dumpval(x260_FloatPath, x290_FloatPath, x2C0_FloatPath, x2F0_VectorPath, x320_VelocityPath, x350);
    dumpval(x354, x358_FloatPath, x388_FloatPath, x3B8_FloatPath, x3E8_FloatPath, x418_FloatPath);
    dumpval(x450, x458_MsgTriggeredEvents);
  }
};

#pragma pack(pop)
