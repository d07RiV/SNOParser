#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(SoundBank::Type) {
  declstruct(SoundSample) {
    char x00_Text[128];
    int x80;
    int x84;
    int x88;
    int x8C;
    int x90;
    int x94;
    int x98;
    int x9C;
    void dumpfunc() {
      dumpval(x00_Text, x80, x84, x88, x8C, x90);
      dumpval(x94, x98, x9C);
    }
  };

  SnoHeader x00_Header;
  SerializeData x0C_SerializeData;
  uint32 x14_;
  Array<SoundSample> x18_SoundSamples = x0C_SerializeData;
  int x20;
  int x24;
  SerializeData x28_SerializeData;
  Array<int64> x30_long = x28_SerializeData;
  int x38;
  int x3C;
  void dumpfunc() {
    dumpval(x00_Header, x18_SoundSamples, x20, x24, x30_long, x38);
    dumpval(x3C);
  }
};

#pragma pack(pop)
