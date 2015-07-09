#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(SceneGroup::Type) {
  declstruct(SceneGroupItem) {
    Sno<Scene> x00_SceneSno;
    int x04;
    GameBalanceId x08_LabelsGameBalanceId;
    void dumpfunc() {
      dumpval(x00_SceneSno, x04, x08_LabelsGameBalanceId);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  SerializeData x10_SerializeData;
  Array<SceneGroupItem> x18_SceneGroupItems = x10_SerializeData;
  int x20;
  uint32 x24_;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x18_SceneGroupItems, x20);
  }
};

#pragma pack(pop)
