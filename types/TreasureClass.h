#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(TreasureClass::Type) {
  declstruct(ItemSpecifierData) {
    int x00_ItemsGameBalanceId;
    int x04;
    int x08_GameBalanceIds[6];
    int x20;
    int x24;
    int x28;
    int x2C;
    int x30;
    int x34;
    void dumpfunc() {
      dumpval(x00_ItemsGameBalanceId, x04, x08_GameBalanceIds, x20, x24, x28);
      dumpval(x2C, x30, x34);
    }
  };

  declstruct(LootDropModifier) {
    int x00;
    int x04_TreasureClassSno;
    float x08_HighPrecisionPercent;
    int x0C;
    int x10_QualityClassesGameBalanceId;
    int x14;
    int x18;
    Sno<Condition> x1C_ConditionSno;
    ItemSpecifierData x20_ItemSpecifierData;
    int x58;
    int x5C;
    int x60_int[5];
    int x74;
    int x78;
    float x7C;
    void dumpfunc() {
      dumpval(x00, x04_TreasureClassSno, x08_HighPrecisionPercent, x0C, x10_QualityClassesGameBalanceId, x14);
      dumpval(x18, x1C_ConditionSno, x20_ItemSpecifierData, x58, x5C, x60_int);
      dumpval(x74, x78, x7C);
    }
  };

  SnoHeader x00_Header;
  float x0C_HighPrecisionPercent;
  int x10;
  int x14;
  Array<LootDropModifier> x18_LootDropModifiers = x20_SerializeData;
  SerializeData x20_SerializeData;
  void dumpfunc() {
    dumpval(x00_Header, x0C_HighPrecisionPercent, x10, x14, x18_LootDropModifiers);
  }
};

#pragma pack(pop)
