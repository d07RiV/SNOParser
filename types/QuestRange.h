#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(QuestRange::Type) {
  declstruct(QuestTime) {
    Sno<Quest> x00_QuestSno;
    int x04;
    void dumpfunc() {
      dumpval(x00_QuestSno, x04);
    }
  };

  declstruct(QuestRangeData) {
    QuestTime x00_QuestTime;
    QuestTime x08_QuestTime;
    void dumpfunc() {
      dumpval(x00_QuestTime, x08_QuestTime);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  Array<QuestRangeData> x10_QuestRanges = x18_SerializeData;
  SerializeData x18_SerializeData;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10_QuestRanges);
  }
};

#pragma pack(pop)
