#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Conversation::Type) {
  declstruct(ConvLocaleDisplayTimes) {
    int x00_int[12];
    void dumpfunc() {
      dumpval(x00_int);
    }
  };

  declstruct(ConversationTreeNode) {
    int x000;
    int x004;
    int x008;
    int x00C_Enum; // - None - = -1, Player = 0, Primary NPC = 1, Alt. NPC 1 = 2, Alt. NPC 2 = 3, Alt. NPC 3 = 4, Alt. NPC 4 = 5, Templar Follower = 6, Scoundrel Follower = 7, Enchantress Follower = 8
    int x010_Enum; // - None - = -1, Player = 0, Primary NPC = 1, Alt. NPC 1 = 2, Alt. NPC 2 = 3, Alt. NPC 3 = 4, Alt. NPC 4 = 5, Templar Follower = 6, Scoundrel Follower = 7, Enchantress Follower = 8
    int x014;
    float x018_Time;
    int x01C;
    int x020;
    int x024;
    ConvLocaleDisplayTimes x028_ConvLocaleDisplayTimes[19];
    int x3B8;
    int x3BC_Enum; // - None - = -1, Equal To = 0, Less Than = 1, Greater Than = 2, Less Than Or Equal To = 3, Greater Than Or Equal To = 4, Not Equal To = 5
    int x3C0;
    int x3C4;
    int x3C8_Enum; // - None - = -1, Incremented By = 0, Decremented By = 1, Set To = 2
    int x3CC;
    int x3D0;
    int x3D4;
    Array<ConversationTreeNode> x3D8_ConversationTreeNodes = x3E0_SerializeData;
    SerializeData x3E0_SerializeData;
    Array<ConversationTreeNode> x3E8_ConversationTreeNodes = x3F0_SerializeData;
    SerializeData x3F0_SerializeData;
    Array<ConversationTreeNode> x3F8_ConversationTreeNodes = x400_SerializeData;
    SerializeData x400_SerializeData;
    void dumpfunc() {
      dumpval(x000, x004, x008, x00C_Enum, x010_Enum, x014);
      dumpval(x018_Time, x01C, x020, x024, x028_ConvLocaleDisplayTimes, x3B8);
      dumpval(x3BC_Enum, x3C0, x3C4, x3C8_Enum, x3CC, x3D0);
      dumpval(x3D4, x3D8_ConversationTreeNodes, x3E8_ConversationTreeNodes, x3F8_ConversationTreeNodes);
    }
  };

  SnoHeader x000_Header;
  int x00C_Enum; // Quest Event = 13, Quest Float = 12, Quest Standard = 11, Talk Menu Gossip = 10, Ambient Gossip = 9, Lore Book = 8, Global Float = 7, Global Chatter = 6, Player Callout = 5, Follower Callout = 4, Follower Banter = 3, Ambient Float = 2, Player Emote = 1, Follower Soundset = 0
  int x010;
  Sno<Conversation> x014_ConversationSno;
  int x018_Snos[3];
  int x024;
  char x028_Text[128];
  Sno<Actor> x0A8_ActorSno;
  Sno<Actor> x0AC_ActorSno;
  Sno<Actor> x0B0_ActorSno;
  Sno<Actor> x0B4_ActorSno;
  Sno<Actor> x0B8_ActorSno;
  int x0BC;
  Array<ConversationTreeNode> x0C0_ConversationTreeNodes = x0C8_SerializeData;
  SerializeData x0C8_SerializeData;
  Array<uint8> x0D0_byte = x0D8_SerializeData;
  SerializeData x0D8_SerializeData;
  uint32 x0E0_;
  uint32 x0E4_;
  uint32 x0E8_;
  uint32 x0EC_;
  uint32 x0F0_;
  uint32 x0F4_;
  uint32 x0F8_;
  uint32 x0FC_;
  uint32 x100_;
  uint32 x104_;
  Sno<BossEncounter> x108_BossEncounterSno;
  int x10C;
  void dumpfunc() {
    dumpval(x000_Header, x00C_Enum, x010, x014_ConversationSno, x018_Snos, x024);
    dumpval(x028_Text, x0A8_ActorSno, x0AC_ActorSno, x0B0_ActorSno, x0B4_ActorSno, x0B8_ActorSno);
    dumpval(x0BC, x0C0_ConversationTreeNodes, x0D0_byte, x108_BossEncounterSno, x10C);
  }
};

#pragma pack(pop)
