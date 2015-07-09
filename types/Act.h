#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Act::Type) {
  declstruct(ActQuestInfo) {
    Sno<Quest> x00_QuestSno;
    void dumpfunc() {
      dumpval(x00_QuestSno);
    }
  };

  declstruct(DT_IVECTOR2D) {
    int x00;
    int x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  declstruct(WaypointInfo) {
    int x00;
    Sno<Worlds> x04_WorldsSno;
    Sno<LevelArea> x08_LevelAreaSno;
    int x0C;
    int x10;
    int x14;
    Sno<QuestRange> x18_QuestRangeSno;
    int x1C;
    DT_IVECTOR2D x20_DT_IVECTOR2D;
    uint32 x24_;
    void dumpfunc() {
      dumpval(x00, x04_WorldsSno, x08_LevelAreaSno, x0C, x10, x14);
      dumpval(x18_QuestRangeSno, x1C, x20_DT_IVECTOR2D);
    }
  };

  declstruct(ResolvedPortalDestination) {
    Sno<Worlds> x00_WorldsSno;
    int x04;
    Sno<LevelArea> x08_LevelAreaSno;
    void dumpfunc() {
      dumpval(x00_WorldsSno, x04, x08_LevelAreaSno);
    }
  };

  declstruct(ActStartLocOverride) {
    ResolvedPortalDestination x00_ResolvedPortalDestination;
    Sno<QuestRange> x0C_QuestRangeSno;
    Sno<Worlds> x10_WorldsSno;
    int x14;
    void dumpfunc() {
      dumpval(x00_ResolvedPortalDestination, x0C_QuestRangeSno, x10_WorldsSno, x14);
    }
  };

  SnoHeader x0000_Header;
  SerializeData x000C_SerializeData;
  uint32 x014_;
  Array<ActQuestInfo> x0018_ActQuestInfos = x000C_SerializeData;
  WaypointInfo x0020_WaypointInfos[100];
  ResolvedPortalDestination x0FC0_ResolvedPortalDestination;
  ActStartLocOverride x0FCC_ActStartLocOverrides[6];
  uint32 x105C_;
  void dumpfunc() {
    dumpval(x0000_Header, x0018_ActQuestInfos, x0020_WaypointInfos, x0FC0_ResolvedPortalDestination, x0FCC_ActStartLocOverrides);
  }
};

#pragma pack(pop)
