#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Lore::Type) {
  SnoHeader x00_Header;
  int x0C;
  int x10;
  int x14;
  int x18;
  Sno<Conversation> x1C_ConversationSno;
  int x20;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x14, x18, x1C_ConversationSno);
    dumpval(x20);
  }
};

#pragma pack(pop)
