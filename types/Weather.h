#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Weather::Type) {
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

  declstruct(DT_RGBACOLOR) {
    uint8 x00;
    uint8 x01;
    uint8 x02;
    uint8 x03;
    void dumpfunc() {
      dumpval(x00, x01, x02, x03);
    }
  };

  declstruct(WeatherStateParams) {
    int x00;
    DT_RGBACOLOR x04_DT_RGBACOLOR;
    uint8 x07_;
    float x08;
    float x0C;
    float x10;
    float x14;
    DT_RGBACOLOR x18_DT_RGBACOLOR;
    uint8 x1B_;
    float x1C;
    float x20;
    float x24;
    Sno<Light> x28_LightSno;
    Sno<Light> x2C_LightSno;
    float x30;
    float x34;
    Sno<Light> x38_LightSno;
    Sno<Light> x3C_LightSno;
    Sno<Light> x40_LightSno;
    void dumpfunc() {
      dumpval(x00, x04_DT_RGBACOLOR, x08, x0C, x10, x14);
      dumpval(x18_DT_RGBACOLOR, x1C, x20, x24, x28_LightSno, x2C_LightSno);
      dumpval(x30, x34, x38_LightSno, x3C_LightSno, x40_LightSno);
    }
  };

  declstruct(ColorCorrectionParams) {
    Sno<Textures> x00_TexturesSno;
    Sno<Textures> x04_TexturesSno;
    Sno<Textures> x08_TexturesSno;
    void dumpfunc() {
      dumpval(x00_TexturesSno, x04_TexturesSno, x08_TexturesSno);
    }
  };

  SnoHeader x000_Header;
  int x00C;
  float x010_Time;
  uint32 x014_;
  VelocityVectorPath x018_VelocityVectorPath;
  VelocityVectorPath x048_VelocityVectorPath;
  float x078_Time;
  uint32 x07C_;
  FloatPath x080_FloatPath;
  Sno<Particle> x0B0_ParticleSno;
  Sno<Particle> x0B4_ParticleSno;
  Sno<Sound> x0B8_SoundSno;
  Sno<Actor> x0BC_ActorSno;
  float x0C0;
  float x0C4_Time;
  float x0C8_Time;
  float x0CC_Time;
  Sno<Light> x0D0_LightSno;
  Sno<EffectGroup> x0D4_EffectGroupSno;
  Sno<Sound> x0D8_SoundSno;
  float x0DC;
  float x0E0;
  float x0E4;
  WeatherStateParams x0E8_WeatherStateParams;
  WeatherStateParams x12C_WeatherStateParams;
  float x170;
  ColorCorrectionParams x174_ColorCorrectionParams;
  Array<int> x180_Snos = x188_SerializeData;
  SerializeData x188_SerializeData;
  int x190;
  int x194;
  void dumpfunc() {
    dumpval(x000_Header, x00C, x010_Time, x018_VelocityVectorPath, x048_VelocityVectorPath, x078_Time);
    dumpval(x080_FloatPath, x0B0_ParticleSno, x0B4_ParticleSno, x0B8_SoundSno, x0BC_ActorSno, x0C0);
    dumpval(x0C4_Time, x0C8_Time, x0CC_Time, x0D0_LightSno, x0D4_EffectGroupSno, x0D8_SoundSno);
    dumpval(x0DC, x0E0, x0E4, x0E8_WeatherStateParams, x12C_WeatherStateParams, x170);
    dumpval(x174_ColorCorrectionParams, x180_Snos, x190, x194);
  }
};

#pragma pack(pop)
