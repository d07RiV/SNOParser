#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(LevelArea::Type) {
  declstruct(SNOName) {
    int x00_SnoGroup;
    int x04_SnoNameHandle;
    void dumpfunc() {
      dumpval(x00_SnoGroup, x04_SnoNameHandle);
    }
  };

  declstruct(GizmoLocSpawnChoice) {
    SNOName x00_SNOName;
    int x08;
    float x0C;
    int x10;
    void dumpfunc() {
      dumpval(x00_SNOName, x08, x0C, x10);
    }
  };

  declstruct(GizmoLocSpawnEntry) {
    int x00;
    int x04;
    int x08;
    int x0C;
    float x10_HighPrecisionPercent;
    Sno<Condition> x14_ConditionSno;
    Array<GizmoLocSpawnChoice> x18_GizmoLocSpawnChoices = x20_SerializeData;
    SerializeData x20_SerializeData;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10_HighPrecisionPercent, x14_ConditionSno);
      dumpval(x18_GizmoLocSpawnChoices);
    }
  };

  declstruct(GizmoLocSpawnType) {
    Array<GizmoLocSpawnEntry> x00_GizmoLocSpawnEntries = x08_SerializeData;
    SerializeData x08_SerializeData;
    void dumpfunc() {
      dumpval(x00_GizmoLocSpawnEntries);
    }
  };

  declstruct(GizmoLocSet) {
    GizmoLocSpawnType x000_GizmoLocSpawnTypes[52];
    void dumpfunc() {
      dumpval(x000_GizmoLocSpawnTypes);
    }
  };

  declstruct(LevelAreaSpawnItem) {
    SNOName x00_SNOName;
    int x08_Enum; //  = 0, Champion = 1, Rare = 2, Minion = 3, Unique = 4, Hireling = 5, Boss = 7
    int x0C;
    int x10;
    int x14;
    int x18;
    float x1C;
    void dumpfunc() {
      dumpval(x00_SNOName, x08_Enum, x0C, x10, x14, x18);
      dumpval(x1C);
    }
  };

  declstruct(LevelAreaSpawnGroup) {
    int x00_Enum; // Count per 100 sq ft = 0, Exactly = 1
    float x04;
    float x08;
    int x0C;
    int x10;
    uint32 x14_;
    Array<LevelAreaSpawnItem> x18_LevelAreaSpawnItems = x20_SerializeData;
    SerializeData x20_SerializeData;
    int x28;
    int x2C;
    Sno<Condition> x30_ConditionSno;
    uint32 x34_;
    void dumpfunc() {
      dumpval(x00_Enum, x04, x08, x0C, x10, x18_LevelAreaSpawnItems);
      dumpval(x28, x2C, x30_ConditionSno);
    }
  };

  declstruct(LevelAreaSpawnPopulation) {
    char x00_Text[64];
    int x40;
    float x44;
    float x48;
    int x4C_int[4];
    int x5C_int[4];
    int x6C;
    Array<LevelAreaSpawnGroup> x70_LevelAreaSpawnGroups = x78_SerializeData;
    SerializeData x78_SerializeData;
    Array<int> x80_Snos = x88_SerializeData;
    SerializeData x88_SerializeData;
    Sno<Encounter> x90_EncounterSno;
    uint32 x94_;
    void dumpfunc() {
      dumpval(x00_Text, x40, x44, x48, x4C_int, x5C_int);
      dumpval(x6C, x70_LevelAreaSpawnGroups, x80_Snos, x90_EncounterSno);
    }
  };

  declstruct(LevelAreaServerData) {
    Sno<LevelArea> x000_LevelAreaSno;
    uint32 x004_;
    GizmoLocSet x008_GizmoLocSet;
    Sno<LevelArea> x348_LevelAreaSno;
    int x34C;
    Array<LevelAreaSpawnPopulation> x350_LevelAreaSpawnPopulations = x358_SerializeData;
    SerializeData x358_SerializeData;
    void dumpfunc() {
      dumpval(x000_LevelAreaSno, x008_GizmoLocSet, x348_LevelAreaSno, x34C, x350_LevelAreaSpawnPopulations);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  Sno<LevelArea> x10_LevelAreaSno;
  Sno<LevelArea> x14_LevelAreaSno;
  Array<LevelAreaServerData> x18_LevelAreaServerDatas = x20_SerializeData;
  SerializeData x20_SerializeData;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10_LevelAreaSno, x14_LevelAreaSno, x18_LevelAreaServerDatas);
  }
};

#pragma pack(pop)
