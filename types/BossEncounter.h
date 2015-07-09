#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(BossEncounter::Type) {
  SnoHeader x00_Header;
  int x0C;
  int x10;
  int x14;
  int x18;
  int x1C;
  int x20;
  int x24;
  int x28;
  int x2C;
  int x30;
  int x34;
  int x38;
  float x3C;
  Sno<QuestRange> x40_QuestRangeSno;
  int x44_Snos[4];
  int x54_Snos[3];
  Sno<LevelArea> x60_LevelAreaSno;
  Sno<Actor> x64_ActorSno;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x14, x18, x1C);
    dumpval(x20, x24, x28, x2C, x30, x34);
    dumpval(x38, x3C, x40_QuestRangeSno, x44_Snos, x54_Snos, x60_LevelAreaSno);
    dumpval(x64_ActorSno);
  }
};

#pragma pack(pop)
