#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Recipe::Type) {
  declstruct(ItemSpecifierData) {
    GameBalanceId x00_ItemsGameBalanceId;
    int x04;
    GameBalanceId x08_GameBalanceIds[6];
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

  SnoHeader x00_Header;
  ItemSpecifierData x0C_ItemSpecifierData;
  int x44;
  void dumpfunc() {
    dumpval(x00_Header, x0C_ItemSpecifierData, x44);
  }
};

#pragma pack(pop)
