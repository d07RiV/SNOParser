#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Quest::Type) {
  declstruct(QuestStepFailureCondition) {
    int x00_Enum; // Monster Died = 0, Player Died = 1, Actor Died = 2, Timed Event Expired = 3, Item Used = 4, Game Flag Set = 5, Player Flag Set = 6, Event Received = 7
    int x04;
    int x08;
    SNOName x0C_SNOName;
    SNOName x14_SNOName;
    GameBalanceId x1C_GameBalanceId;
    GameBalanceId x20_GameBalanceId;
    uint32 x24_;
    Text x28_Text = x30_SerializeData;
    SerializeData x30_SerializeData;
    Text x38_Text = x40_SerializeData;
    SerializeData x40_SerializeData;
    void dumpfunc() {
      dumpval(x00_Enum, x04, x08, x0C_SNOName, x14_SNOName, x1C_GameBalanceId);
      dumpval(x20_GameBalanceId, x28_Text, x38_Text);
    }
  };

  declstruct(QuestStepFailureConditionSet) {
    Array<QuestStepFailureCondition> x00_QuestStepFailureConditions = x08_SerializeData;
    SerializeData x08_SerializeData;
    void dumpfunc() {
      dumpval(x00_QuestStepFailureConditions);
    }
  };

  declstruct(QuestStepObjective) {
    int x00;
    int x04_Enum; // Had Conversation = 0, Possess Item = 1, Kill Monster = 2, Interact With Actor = 3, Enter Level Area = 4, Enter Scene = 5, Enter World = 6, Enter Trigger = 7, Complete Quest = 8, Player Flag Set = 9, Timed Event Expired = 10, Kill Group = 11, Game Flag Set = 12, Event Received = 13, Kill Monster From Group = 14, Kill Monster From Family = 15, Kill Elite Pack In Level Area = 16, Kill Any Monster In Level Area = 17, Kill All Monsters In World = 18, HQ Act Reagents Collected = 19
    int x08;
    int x0C;
    SNOName x10_SNOName;
    SNOName x18_SNOName;
    GameBalanceId x20_GameBalanceId;
    GameBalanceId x24_GameBalanceId;
    Text x28_Text = x30_SerializeData;
    SerializeData x30_SerializeData;
    Text x38_Text = x40_SerializeData;
    SerializeData x40_SerializeData;
    int x48;
    int x4C;
    GameBalanceId x50_ItemsGameBalanceId;
    uint32 x54_;
    void dumpfunc() {
      dumpval(x00, x04_Enum, x08, x0C, x10_SNOName, x18_SNOName);
      dumpval(x20_GameBalanceId, x24_GameBalanceId, x28_Text, x38_Text, x48, x4C);
      dumpval(x50_ItemsGameBalanceId);
    }
  };

  declstruct(QuestStepObjectiveSet) {
    int x00;
    uint32 x04_;
    Array<QuestStepObjective> x08_QuestStepObjectives = x10_SerializeData;
    SerializeData x10_SerializeData;
    void dumpfunc() {
      dumpval(x00, x08_QuestStepObjectives);
    }
  };

  declstruct(QuestUnassignedStep) {
    int x00;
    int x04;
    Array<QuestStepObjectiveSet> x08_QuestStepObjectiveSets = x10_SerializeData;
    SerializeData x10_SerializeData;
    Array<QuestStepFailureConditionSet> x18_QuestStepFailureConditionSets = x20_SerializeData;
    SerializeData x20_SerializeData;
    void dumpfunc() {
      dumpval(x00, x04, x08_QuestStepObjectiveSets, x18_QuestStepFailureConditionSets);
    }
  };

  declstruct(QuestStepBonusObjectiveSet) {
    int x00;
    int x04;
    int x08;
    int x0C;
    int x10;
    uint32 x14_;
    Array<QuestStepObjective> x18_QuestStepObjectives = x20_SerializeData;
    SerializeData x20_SerializeData;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x18_QuestStepObjectives);
    }
  };

  declstruct(QuestStep) {
    Text x00_Text = x08_SerializeData;
    SerializeData x08_SerializeData;
    int x10;
    int x14;
    int x18_Enum; // - No Item - = 0, Shared Recipe = 1, Class Recipe = 2, Treasure Class = 3
    int x1C_Snos[6];
    int x34_TreasureClassSno;
    int x38;
    int x3C;
    int x40_Enum; // - No Item - = 0, Shared Recipe = 1, Class Recipe = 2, Treasure Class = 3
    int x44_Snos[6];
    int x5C_TreasureClassSno;
    int x60;
    int x64;
    Sno<Power> x68_PowerSno;
    int x6C_Snos[2];
    uint32 x74_;
    Array<QuestStepObjectiveSet> x78_QuestStepObjectiveSets = x80_SerializeData;
    SerializeData x80_SerializeData;
    Array<QuestStepBonusObjectiveSet> x88_QuestStepBonusObjectiveSets = x90_SerializeData;
    SerializeData x90_SerializeData;
    Array<QuestStepFailureConditionSet> x98_QuestStepFailureConditionSets = xA0_SerializeData;
    SerializeData xA0_SerializeData;
    void dumpfunc() {
      dumpval(x00_Text, x10, x14, x18_Enum, x1C_Snos, x34_TreasureClassSno);
      dumpval(x38, x3C, x40_Enum, x44_Snos, x5C_TreasureClassSno, x60);
      dumpval(x64, x68_PowerSno, x6C_Snos, x78_QuestStepObjectiveSets, x88_QuestStepBonusObjectiveSets, x98_QuestStepFailureConditionSets);
    }
  };

  declstruct(QuestCompletionStep) {
    Text x00_Text = x08_SerializeData;
    SerializeData x08_SerializeData;
    int x10;
    int x14;
    void dumpfunc() {
      dumpval(x00_Text, x10, x14);
    }
  };

  declstruct(BountyData) {
    int x00_Enum; // Invalid = -1, A1 = 0, A2 = 100, A3 = 200, A4 = 300, A5 = 400, OpenWorld = 3000, WeeklyChallenge = 4000, Test = 1000
    int x04_Enum; // - None - = -1, Kill Unique = 0, Kill Boss = 1, Complete Event = 2, Clear Dungeon = 3, Camps = 4, HQ - Color Orbs = 5, HQ - Hot Cold = 6
    int x08;
    Sno<LevelArea> x0C_LevelAreaSno;
    Sno<Worlds> x10_WorldsSno;
    Sno<Quest> x14_QuestSno;
    Sno<Worlds> x18_WorldsSno;
    Sno<Actor> x1C_ActorSno;
    Sno<Worlds> x20_WorldsSno;
    Sno<LevelArea> x24_LevelAreaSno;
    Sno<Scene> x28_SceneSno;
    Sno<Worlds> x2C_WorldsSno;
    GameBalanceId x30_LabelsGameBalanceId;
    Sno<Adventure> x34_AdventureSno;
    Sno<Worlds> x38_WorldsSno;
    Sno<LevelArea> x3C_LevelAreaSno;
    Sno<Encounter> x40_EncounterSno;
    Sno<Scene> x44_SceneSno;
    Sno<Worlds> x48_WorldsSno;
    GameBalanceId x4C_LabelsGameBalanceId;
    int x50_54Sno;
    void dumpfunc() {
      dumpval(x00_Enum, x04_Enum, x08, x0C_LevelAreaSno, x10_WorldsSno, x14_QuestSno);
      dumpval(x18_WorldsSno, x1C_ActorSno, x20_WorldsSno, x24_LevelAreaSno, x28_SceneSno, x2C_WorldsSno);
      dumpval(x30_LabelsGameBalanceId, x34_AdventureSno, x38_WorldsSno, x3C_LevelAreaSno, x40_EncounterSno, x44_SceneSno);
      dumpval(x48_WorldsSno, x4C_LabelsGameBalanceId, x50_54Sno);
    }
  };

  SnoHeader x000_Header;
  int x00C_Enum; // Main Quest = 0, Event = 2, Challenge = 4, Bounty = 5, Horadric Quest = 6
  int x010;
  int x014;
  int x018;
  int x01C;
  int x020;
  int x024;
  QuestUnassignedStep x028_QuestUnassignedStep;
  Array<QuestStep> x050_QuestSteps = x058_SerializeData;
  SerializeData x058_SerializeData;
  Array<QuestCompletionStep> x060_QuestCompletionSteps = x068_SerializeData;
  SerializeData x068_SerializeData;
  int x070_Snos[18];
  Sno<Worlds> x0B8_WorldsSno;
  int x0BC_Enum; // - None - = -1, Timed Dungeon = 0, Wave Fight = 1, Horde = 2, Zapper = 3, Goblin Hunt = 4
  BountyData x0C0_BountyData;
  uint32 x114_;
  void dumpfunc() {
    dumpval(x000_Header, x00C_Enum, x010, x014, x018, x01C);
    dumpval(x020, x024, x028_QuestUnassignedStep, x050_QuestSteps, x060_QuestCompletionSteps, x070_Snos);
    dumpval(x0B8_WorldsSno, x0BC_Enum, x0C0_BountyData);
  }
};

#pragma pack(pop)
