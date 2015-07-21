#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Material::Type) {
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

  SnoHeader x00_Header;
  int x0C;
  SerializeData x10_SerializeData;
  Array<int> x18_TagMap = x10_SerializeData; // type: 0x000A0000
  UberMaterial x20_UberMaterial;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x18_TagMap, x20_UberMaterial);
  }
};

#pragma pack(pop)
