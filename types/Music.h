#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Music::Type) {
  declstruct(PlaylistEntry) {
    Sno<SoundBank> x00_SoundBankSno;
    char x04_Text[132];
    float x88;
    int x8C;
    int x90;
    int x94;
    int x98;
    int x9C;
    int xA0;
    int xA4;
    int xA8;
    int xAC;
    int xB0;
    int xB4;
    int xB8;
    float xBC;
    float xC0;
    uint32 xC4_;
    uint32 xC8_;
    void dumpfunc() {
      dumpval(x00_SoundBankSno, x04_Text, x88, x8C, x90, x94);
      dumpval(x98, x9C, xA0, xA4, xA8, xAC);
      dumpval(xB0, xB4, xB8, xBC, xC0);
    }
  };

  declstruct(RandomMusicSoundParams) {
    Sno<Sound> x00_SoundSno;
    float x04_Time;
    float x08_Time;
    float x0C_Time;
    void dumpfunc() {
      dumpval(x00_SoundSno, x04_Time, x08_Time, x0C_Time);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  float x10;
  float x14_Time;
  float x18_Time;
  SerializeData x1C_SerializeData;
  int x24;
  Array<PlaylistEntry> x28_PlaylistEntries = x1C_SerializeData;
  RandomMusicSoundParams x30_RandomMusicSoundParams[2];
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x14_Time, x18_Time, x24);
    dumpval(x28_PlaylistEntries, x30_RandomMusicSoundParams);
  }
};

#pragma pack(pop)
