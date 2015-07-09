#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Light::Type) {
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

  declstruct(AngleNode) {
    float x00_Angle;
    float x04_Angle;
    float x08;
    void dumpfunc() {
      dumpval(x00_Angle, x04_Angle, x08);
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

  declstruct(AnglePath) {
    InterpolationPathHeader x00_InterpolationPathHeader;
    uint32 x1C_;
    Array<AngleNode> x20_AngleNodes = x28_SerializeData;
    SerializeData x28_SerializeData;
    void dumpfunc() {
      dumpval(x00_InterpolationPathHeader, x20_AngleNodes);
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

  declstruct(LightStateParams) {
    FloatPath x000_FloatPath;
    FloatPath x030_FloatPath;
    FloatPath x060_FloatPaths[3];
    AnglePath x0F0_AnglePath;
    AnglePath x120_AnglePath;
    ColorPath x150_ColorPath;
    FloatPath x180_FloatPath;
    ColorPath x1B0_ColorPath;
    FloatPath x1E0_FloatPath;
    ColorPath x210_ColorPath;
    FloatPath x240_FloatPath;
    void dumpfunc() {
      dumpval(x000_FloatPath, x030_FloatPath, x060_FloatPaths, x0F0_AnglePath, x120_AnglePath, x150_ColorPath);
      dumpval(x180_FloatPath, x1B0_ColorPath, x1E0_FloatPath, x210_ColorPath, x240_FloatPath);
    }
  };

  SnoHeader x000_Header;
  int x00C;
  int x010;
  int x014;
  MaterialTexture x018_MaterialTexture;
  float x0B0_Time;
  int x0B4;
  float x0B8;
  uint32 x0BC_;
  LightStateParams x0C0_LightStateParams;
  LightStateParams x330_LightStateParams;
  void dumpfunc() {
    dumpval(x000_Header, x00C, x010, x014, x018_MaterialTexture, x0B0_Time);
    dumpval(x0B4, x0B8, x0C0_LightStateParams, x330_LightStateParams);
  }
};

#pragma pack(pop)
