#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Sound::Type) {
  declstruct(SoundPermutation) {
    Sno<SoundBank> x00_SoundBankSno;
    char x04_Text[132];
    int x88;
    float x8C;
    float x90;
    float x94;
    float x98;
    float x9C_Time;
    float xA0_Time;
    int xA4;
    int xA8;
    float xAC;
    float xB0;
    float xB4_Time;
    uint32 xB8_;
    uint32 xBC_;
    void dumpfunc() {
      dumpval(x00_SoundBankSno, x04_Text, x88, x8C, x90, x94);
      dumpval(x98, x9C_Time, xA0_Time, xA4, xA8, xAC);
      dumpval(xB0, xB4_Time);
    }
  };

  declstruct(DspParam) {
    float x00;
    float x04;
    int x08;
    int x0C;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C);
    }
  };

  declstruct(DspEffect) {
    int x000_Enum; // Chorus = 0, Compressor = 1, Distortion = 2, Echo = 3, Flange = 4, High Pass Filter = 5, Low Pass Filter = 6, Low Pass Filter (Simple) = 7, Parametric EQ = 8, SFX Reverb = 9
    int x004;
    DspParam x008_DspParam;
    DspParam x018_DspParam;
    DspParam x028_DspParam;
    DspParam x038_DspParam;
    DspParam x048_DspParam;
    DspParam x058_DspParam;
    DspParam x068_DspParam;
    DspParam x078_DspParam;
    DspParam x088_DspParam;
    DspParam x098_DspParam;
    DspParam x0A8_DspParam;
    DspParam x0B8_DspParam;
    DspParam x0C8_DspParam;
    DspParam x0D8_DspParam;
    DspParam x0E8_DspParam;
    uint32 x0F8_;
    uint32 x0FC_;
    uint32 x100_;
    uint32 x104_;
    void dumpfunc() {
      dumpval(x000_Enum, x004, x008_DspParam, x018_DspParam, x028_DspParam, x038_DspParam);
      dumpval(x048_DspParam, x058_DspParam, x068_DspParam, x078_DspParam, x088_DspParam, x098_DspParam);
      dumpval(x0A8_DspParam, x0B8_DspParam, x0C8_DspParam, x0D8_DspParam, x0E8_DspParam);
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

  declstruct(RTPCParams) {
    int x00;
    InterpolationScalar x04_InterpolationScalar;
    int x10;
    float x14;
    void dumpfunc() {
      dumpval(x00, x04_InterpolationScalar, x10, x14);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  int x10;
  float x14;
  float x18;
  float x1C;
  int x20;
  float x24;
  float x28;
  float x2C;
  float x30;
  float x34_Impulse;
  float x38_Impulse;
  float x3C;
  float x40;
  SerializeData x44_SerializeData;
  int x4C;
  Array<SoundPermutation> x50_SoundPermutations = x44_SerializeData;
  int x58;
  int x5C;
  int x60;
  uint32 x64_;
  Array<DspEffect> x68_DspEffects = x70_SerializeData;
  SerializeData x70_SerializeData;
  Array<RTPCParams> x78_RTPCParams = x80_SerializeData;
  SerializeData x80_SerializeData;
  float x88_Time;
  float x8C_Time;
  uint32 x90_;
  uint32 x94_;
  uint32 x98_;
  uint32 x9C_;
  uint32 xA0_;
  uint32 xA4_;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x14, x18, x1C);
    dumpval(x20, x24, x28, x2C, x30, x34_Impulse);
    dumpval(x38_Impulse, x3C, x40, x4C, x50_SoundPermutations, x58);
    dumpval(x5C, x60, x68_DspEffects, x78_RTPCParams, x88_Time, x8C_Time);
  }
};

#pragma pack(pop)
