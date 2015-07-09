#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(ConversationList::Type) {
  declstruct(ConversationListEntry) {
    Sno<Conversation> x000_ConversationSno;
    int x004;
    int x008;
    int x00C;
    int x010_ItemsGameBalanceId;
    char x014_Text[128];
    char x094_Text[128];
    Sno<Quest> x114_QuestSno;
    int x118;
    Sno<Quest> x11C_QuestSno;
    Sno<Quest> x120_QuestSno;
    Sno<Quest> x124_QuestSno;
    Sno<QuestRange> x128_QuestRangeSno;
    int x12C_Snos[6];
    int x144;
    int x148;
    void dumpfunc() {
      dumpval(x000_ConversationSno, x004, x008, x00C, x010_ItemsGameBalanceId, x014_Text);
      dumpval(x094_Text, x114_QuestSno, x118, x11C_QuestSno, x120_QuestSno, x124_QuestSno);
      dumpval(x128_QuestRangeSno, x12C_Snos, x144, x148);
    }
  };

  SnoHeader x00_Header;
  uint32 x0C_;
  Array<ConversationListEntry> x10_ConversationListEntries = x18_SerializeData;
  SerializeData x18_SerializeData;
  void dumpfunc() {
    dumpval(x00_Header, x10_ConversationListEntries);
  }
};

#pragma pack(pop)
