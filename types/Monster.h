#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Monster::Type) {
  declstruct(MonsterData) {
    float x00;
    float x04;
    float x08;
    float x0C;
    int x10;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10);
    }
  };

  declstruct(HealthDropInfo) {
    float x00;
    GameBalanceId x04_ItemsGameBalanceId;
    int x08;
    void dumpfunc() {
      dumpval(x00, x04_ItemsGameBalanceId, x08);
    }
  };

  declstruct(SkillDeclaration) {
    Sno<Power> x00_PowerSno;
    int x04;
    void dumpfunc() {
      dumpval(x00_PowerSno, x04);
    }
  };

  declstruct(MonsterSkillDeclaration) {
    float x00;
    float x04;
    int x08;
    float x0C;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C);
    }
  };

  declstruct(MonsterMinionSpawnItem) {
    Sno<Actor> x00_ActorSno;
    int x04;
    int x08;
    int x0C;
    int x10;
    void dumpfunc() {
      dumpval(x00_ActorSno, x04, x08, x0C, x10);
    }
  };

  declstruct(MonsterMinionSpawnGroup) {
    float x00;
    int x04;
    Array<MonsterMinionSpawnItem> x08_MonsterMinionSpawnItems = x10_SerializeData;
    SerializeData x10_SerializeData;
    void dumpfunc() {
      dumpval(x00, x04, x08_MonsterMinionSpawnItems);
    }
  };

  declstruct(MonsterChampionSpawnItem) {
    Sno<Actor> x00_ActorSno;
    int x04;
    void dumpfunc() {
      dumpval(x00_ActorSno, x04);
    }
  };

  declstruct(MonsterChampionSpawnGroup) {
    int x00;
    float x04;
    Array<MonsterChampionSpawnItem> x08_MonsterChampionSpawnItems = x10_SerializeData;
    SerializeData x10_SerializeData;
    void dumpfunc() {
      dumpval(x00, x04, x08_MonsterChampionSpawnItems);
    }
  };

  SnoHeader x000_Header;
  int x00C;
  Sno<Actor> x010_ActorSno;
  int x014;
  int x018_Enum; //  = -1, Undead = 0, Demon = 1, Beast = 2, Human = 3, Breakable = 4, Scenery = 5, Ally = 6, Team = 7, Helper = 8, CorruptedAngel = 9, Pandemonium = 10, Adria = 11, Blood Golem = 12
  int x01C_Enum; //  = 0, Fallen = 1, GoatMen = 2, Rogue = 3, Skeleton = 4, Zombie = 5, Spider = 6, Triune = 7, WoodWraith = 8, Human = 9, Animal = 10, TreasureGoblin = 11, CrazedAngel = 12
  int x020_Enum; //  = -1, Big = 3, Standard = 4, Ranged = 5, Swarm = 6, Boss = 7
  MonsterData x024_Monster;
  int x038_Enum; // None = -1, Physical = 0, Fire = 1, Lightning = 2, Cold = 3, Poison = 4, Arcane = 5, Holy = 6
  int x03C;
  int x040;
  float x044_float[146];
  float x28C;
  float x290;
  float x294;
  float x298;
  float x29C;
  float x2A0;
  int x2A4;
  HealthDropInfo x2A8_HealthDropInfo;
  HealthDropInfo x2B4_HealthDropInfo;
  HealthDropInfo x2C0_HealthDropInfo;
  HealthDropInfo x2CC_HealthDropInfo;
  Sno<SkillKit> x2D8_SkillKitSno;
  SkillDeclaration x2DC_SkillDeclarations[8];
  MonsterSkillDeclaration x31C_MonsterSkillDeclarations[8];
  int x39C_TreasureClassSno;
  int x3A0_TreasureClassSno;
  int x3A4_TreasureClassSno;
  int x3A8_TreasureClassSno;
  int x3AC_TreasureClassSno;
  float x3B0;
  float x3B4;
  float x3B8;
  float x3BC;
  int x3C0;
  float x3C4;
  int x3C8;
  int x3CC;
  int x3D0_TreasureClassSno;
  int x3D4_TreasureClassSno;
  Sno<Lore> x3D8_LoreSno;
  int x3DC_Snos[6];
  GameBalanceId x3F4_GameBalanceIds[8];
  int x414_Snos[6];
  int x42C;
  GameBalanceId x430_GameBalanceIds[4];
  GameBalanceId x440_GameBalanceIds[6];
  int x458;
  int x45C;
  int x460;
  int x464_Enum; // Mana = 0, Arcanum = 1, Fury = 2, Spirit = 3, Power = 4, Hatred = 5, Discipline = 6, Faith = 7
  uint32 x468_;
  uint32 x46C_;
  uint32 x470_;
  uint32 x474_;
  uint32 x478_;
  uint32 x47C_;
  SerializeData x480_SerializeData;
  Array<int> x488_TagMap = x480_SerializeData; // type: 0x00120000
  int x490;
  uint32 x494_;
  Array<MonsterMinionSpawnGroup> x498_MonsterMinionSpawnGroups = x4A0_SerializeData;
  SerializeData x4A0_SerializeData;
  int x4A8;
  uint32 x4AC_;
  Array<MonsterChampionSpawnGroup> x4B0_MonsterChampionSpawnGroups = x4B8_SerializeData;
  SerializeData x4B8_SerializeData;
  char x4C0_Text[128];
  void dumpfunc() {
    dumpval(x000_Header, x00C, x010_ActorSno, x014, x018_Enum, x01C_Enum);
    dumpval(x020_Enum, x024_Monster, x038_Enum, x03C, x040, x044_float);
    dumpval(x28C, x290, x294, x298, x29C, x2A0);
    dumpval(x2A4, x2A8_HealthDropInfo, x2B4_HealthDropInfo, x2C0_HealthDropInfo, x2CC_HealthDropInfo, x2D8_SkillKitSno);
    dumpval(x2DC_SkillDeclarations, x31C_MonsterSkillDeclarations, x39C_TreasureClassSno, x3A0_TreasureClassSno, x3A4_TreasureClassSno, x3A8_TreasureClassSno);
    dumpval(x3AC_TreasureClassSno, x3B0, x3B4, x3B8, x3BC, x3C0);
    dumpval(x3C4, x3C8, x3CC, x3D0_TreasureClassSno, x3D4_TreasureClassSno, x3D8_LoreSno);
    dumpval(x3DC_Snos, x3F4_GameBalanceIds, x414_Snos, x42C, x430_GameBalanceIds, x440_GameBalanceIds);
    dumpval(x458, x45C, x460, x464_Enum, x488_TagMap, x490);
    dumpval(x498_MonsterMinionSpawnGroups, x4A8, x4B0_MonsterChampionSpawnGroups, x4C0_Text);
  }
};

#pragma pack(pop)
