#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Condition::Type) {
  declstruct(LoreSubcondition) {
    Sno<Lore> x00_LoreSno;
    int x04;
    void dumpfunc() {
      dumpval(x00_LoreSno, x04);
    }
  };

  declstruct(QuestSubcondition) {
    Sno<Quest> x00_QuestSno;
    int x04;
    int x08;
    int x0C;
    void dumpfunc() {
      dumpval(x00_QuestSno, x04, x08, x0C);
    }
  };

  declstruct(ItemSubcondition) {
    GameBalanceId x00_ItemsGameBalanceId;
    int x04;
    int x08;
    int x0C;
    void dumpfunc() {
      dumpval(x00_ItemsGameBalanceId, x04, x08, x0C);
    }
  };

  declstruct(FollowerSubcondition) {
    int x00_Enum; // - None - = 0, Templar = 1, Scoundrel = 2, Enchantress = 3
    int x04;
    void dumpfunc() {
      dumpval(x00_Enum, x04);
    }
  };

  declstruct(LabelSubcondition) {
    GameBalanceId x00_LabelsGameBalanceId;
    int x04;
    void dumpfunc() {
      dumpval(x00_LabelsGameBalanceId, x04);
    }
  };

  declstruct(SkillSubcondition) {
    Sno<Power> x00_PowerSno;
    int x04;
    int x08;
    void dumpfunc() {
      dumpval(x00_PowerSno, x04, x08);
    }
  };

  declstruct(MonsterSubcondition) {
    Sno<Actor> x00_ActorSno;
    void dumpfunc() {
      dumpval(x00_ActorSno);
    }
  };

  declstruct(GameFlagSubcondition) {
    char x00_Text[128];
    void dumpfunc() {
      dumpval(x00_Text);
    }
  };

  declstruct(PlayerFlagSubcondition) {
    char x00_Text[128];
    void dumpfunc() {
      dumpval(x00_Text);
    }
  };

  declstruct(BuffSubcondition) {
    Sno<Power> x00_PowerSno;
    int x04;
    int x08;
    void dumpfunc() {
      dumpval(x00_PowerSno, x04, x08);
    }
  };

  SnoHeader x000_Header;
  int x00C;
  int x010;
  int x014_int[6];
  int x02C;
  int x030;
  int x034;
  int x038;
  LoreSubcondition x03C_LoreSubconditions[3];
  QuestSubcondition x054_QuestSubconditions[3];
  ItemSubcondition x084_ItemSubconditions[3];
  int x0B4;
  int x0B8;
  int x0BC;
  int x0C0;
  int x0C4;
  int x0C8;
  int x0CC;
  int x0D0;
  uint32 x0D4_;
  int x0D8;
  int x0DC;
  int x0E0;
  int x0E4;
  Sno<Worlds> x0E8_WorldsSno;
  Sno<LevelArea> x0EC_LevelAreaSno;
  Sno<QuestRange> x0F0_QuestRangeSno;
  FollowerSubcondition x0F4_FollowerSubcondition;
  LabelSubcondition x0FC_LabelSubconditions[3];
  SkillSubcondition x114_SkillSubconditions[3];
  int x138;
  int x13C;
  int x140;
  int x144;
  int x148;
  int x14C;
  int x150;
  int x154;
  MonsterSubcondition x158_MonsterSubconditions[15];
  GameFlagSubcondition x194_GameFlagSubconditions[3];
  PlayerFlagSubcondition x314_PlayerFlagSubconditions[3];
  BuffSubcondition x494_BuffSubconditions[3];
  int x4B8;
  int x4BC;
  int x4C0;
  void dumpfunc() {
    dumpval(x000_Header, x00C, x010, x014_int, x02C, x030);
    dumpval(x034, x038, x03C_LoreSubconditions, x054_QuestSubconditions, x084_ItemSubconditions, x0B4);
    dumpval(x0B8, x0BC, x0C0, x0C4, x0C8, x0CC);
    dumpval(x0D0, x0D8, x0DC, x0E0, x0E4, x0E8_WorldsSno);
    dumpval(x0EC_LevelAreaSno, x0F0_QuestRangeSno, x0F4_FollowerSubcondition, x0FC_LabelSubconditions, x114_SkillSubconditions, x138);
    dumpval(x13C, x140, x144, x148, x14C, x150);
    dumpval(x154, x158_MonsterSubconditions, x194_GameFlagSubconditions, x314_PlayerFlagSubconditions, x494_BuffSubconditions, x4B8);
    dumpval(x4BC, x4C0);
  }
};

#pragma pack(pop)
