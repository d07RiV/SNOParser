#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(AmbientSound::Type) {
  declstruct(RandomAmbientSoundParams) {
    Sno<Sound> x00_SoundSno;
    float x04;
    float x08;
    float x0C_Time;
    float x10_Time;
    void dumpfunc() {
      dumpval(x00_SoundSno, x04, x08, x0C_Time, x10_Time);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  Sno<Sound> x10_SoundSno;
  RandomAmbientSoundParams x14_RandomAmbientSoundParams[2];
  Sno<Sound> x3C_SoundSno;
  float x40_Time;
  float x44_Time;
  char x48_Text[64];
  float x88;
  float x8C;
  float x90;
  float x94;
  float x98;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10_SoundSno, x14_RandomAmbientSoundParams, x3C_SoundSno, x40_Time);
    dumpval(x44_Time, x48_Text, x88, x8C, x90, x94);
    dumpval(x98);
  }
};

#pragma pack(pop)
