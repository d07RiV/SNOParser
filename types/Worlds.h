#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Worlds::Type) {
  declstruct(DRLGCommand) {
    char x00_Text[128];
    int x80;
    SerializeData x84_SerializeData;
    uint32 x8C_;
    Array<int> x90_TagMap = x84_SerializeData; // type: 0x00110000
    void dumpfunc() {
      dumpval(x00_Text, x80, x90_TagMap);
    }
  };

  declstruct(CustomTileCell) {
    int x00;
    int x04;
    int x08;
    Sno<Scene> x0C_SceneSno;
    int x10;
    int x14_int[4];
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C_SceneSno, x10, x14_int);
    }
  };

  declstruct(DT_IVECTOR2D) {
    int x00;
    int x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  declstruct(CustomTileInfo) {
    int x00;
    int x04;
    int x08;
    DT_IVECTOR2D x0C_DT_IVECTOR2D;
    uint32 x10_;
    SerializeData x14_SerializeData;
    uint32 x1C_;
    Array<CustomTileCell> x20_CustomTileCells = x14_SerializeData;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C_DT_IVECTOR2D, x20_CustomTileCells);
    }
  };

  declstruct(TileInfo) {
    int x00;
    int x04;
    Sno<Scene> x08_SceneSno;
    int x0C;
    SerializeData x10_SerializeData;
    Array<int> x18_TagMap = x10_SerializeData; // type: 0x000F0000
    CustomTileInfo x20_CustomTileInfo;
    int x48;
    uint32 x4C_;
    void dumpfunc() {
      dumpval(x00, x04, x08_SceneSno, x0C, x18_TagMap, x20_CustomTileInfo);
      dumpval(x48);
    }
  };

  declstruct(DRLGParams) {
    SerializeData x00_SerializeData;
    Array<TileInfo> x08_TileInfos = x00_SerializeData;
    uint32 x10_;
    uint32 x14_;
    uint32 x18_;
    uint32 x1C_;
    uint32 x20_;
    uint32 x24_;
    uint32 x28_;
    uint32 x2C_;
    uint32 x30_;
    uint32 x34_;
    uint32 x38_;
    uint32 x3C_;
    int x40;
    SerializeData x44_SerializeData;
    uint32 x4C_;
    Array<DRLGCommand> x50_DRLGCommands = x44_SerializeData;
    SerializeData x58_SerializeData;
    Array<int> x60_int = x58_SerializeData;
    SerializeData x68_SerializeData;
    Array<int> x70_TagMap = x68_SerializeData; // type: 0x000D0000
    void dumpfunc() {
      dumpval(x08_TileInfos, x40, x50_DRLGCommands, x60_int, x70_TagMap);
    }
  };

  declstruct(LabelCondition) {
    int x00_Enum; // Always = 0, Label Already Set = 2, Game Is Open World = 3
    int x04;
    int x08_int[4];
    void dumpfunc() {
      dumpval(x00_Enum, x04, x08_int);
    }
  };

  declstruct(LabelEntry) {
    GameBalanceId x00_LabelsGameBalanceId;
    int x04;
    float x08;
    int x0C;
    int x10;
    void dumpfunc() {
      dumpval(x00_LabelsGameBalanceId, x04, x08, x0C, x10);
    }
  };

  declstruct(LabelRule) {
    char x00_Text[128];
    LabelCondition x80_LabelCondition;
    int x98;
    int x9C;
    Array<LabelEntry> xA0_LabelEntries = xA8_SerializeData;
    SerializeData xA8_SerializeData;
    void dumpfunc() {
      dumpval(x00_Text, x80_LabelCondition, x98, x9C, xA0_LabelEntries);
    }
  };

  declstruct(LabelRuleSet) {
    int x00;
    uint32 x04_;
    Array<LabelRule> x08_LabelRules = x10_SerializeData;
    SerializeData x10_SerializeData;
    void dumpfunc() {
      dumpval(x00, x08_LabelRules);
    }
  };

  declstruct(SubSceneLabel) {
    GameBalanceId x00_GameBalanceId;
    int x04;
    void dumpfunc() {
      dumpval(x00_GameBalanceId, x04);
    }
  };

  declstruct(SubSceneEntry) {
    Sno<Scene> x00_SceneSno;
    int x04;
    int x08;
    uint32 x0C_;
    Array<SubSceneLabel> x10_SubSceneLabels = x18_SerializeData;
    SerializeData x18_SerializeData;
    void dumpfunc() {
      dumpval(x00_SceneSno, x04, x08, x10_SubSceneLabels);
    }
  };

  declstruct(SubSceneGroup) {
    Sno<Condition> x00_ConditionSno;
    int x04;
    int x08;
    uint32 x0C_;
    Array<SubSceneEntry> x10_SubSceneEntries = x18_SerializeData;
    SerializeData x18_SerializeData;
    void dumpfunc() {
      dumpval(x00_ConditionSno, x04, x08, x10_SubSceneEntries);
    }
  };

  declstruct(SceneCluster) {
    char x00_Text[128];
    int x80;
    int x84;
    Array<SubSceneGroup> x88_SubSceneGroups = x90_SerializeData;
    SerializeData x90_SerializeData;
    SubSceneGroup x98_SubSceneGroup;
    void dumpfunc() {
      dumpval(x00_Text, x80, x84, x88_SubSceneGroups, x98_SubSceneGroup);
    }
  };

  declstruct(SceneClusterSet) {
    int x00;
    uint32 x04_;
    Array<SceneCluster> x08_SceneClusters = x10_SerializeData;
    SerializeData x10_SerializeData;
    void dumpfunc() {
      dumpval(x00, x08_SceneClusters);
    }
  };

  declstruct(DT_VECTOR3D) {
    float x00_X;
    float x04_Y;
    float x08_Z;
    void dumpfunc() {
      dumpval(x00_X, x04_Y, x08_Z);
    }
  };

  declstruct(Quaternion) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    uint32 x08_;
    float x0C;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C);
    }
  };

  declstruct(PRTransform) {
    Quaternion x00_Quaternion;
    DT_VECTOR3D x10_DT_VECTOR3D;
    uint32 x18_;
    void dumpfunc() {
      dumpval(x00_Quaternion, x10_DT_VECTOR3D);
    }
  };

  declstruct(SNOName) {
    int x00_SnoGroup;
    int x04_SnoNameHandle;
    void dumpfunc() {
      dumpval(x00_SnoGroup, x04_SnoNameHandle);
    }
  };

  declstruct(AABB) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    uint32 x08_;
    DT_VECTOR3D x0C_DT_VECTOR3D;
    uint32 x14_;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C_DT_VECTOR3D);
    }
  };

  declstruct(SceneCachedValues) {
    int x00;
    int x04;
    int x08;
    AABB x0C_AABB;
    AABB x24_AABB;
    int x3C_int[4];
    int x4C;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C_AABB, x24_AABB, x3C_int);
      dumpval(x4C);
    }
  };

  declstruct(SceneSpecification) {
    int x00;
    DT_IVECTOR2D x04_DT_IVECTOR2D;
    uint32 x08_;
    int x0C_Snos[4];
    Sno<Worlds> x1C_WorldsSno;
    int x20;
    Sno<LevelArea> x24_LevelAreaSno;
    Sno<Worlds> x28_WorldsSno;
    int x2C;
    Sno<LevelArea> x30_LevelAreaSno;
    Sno<Music> x34_MusicSno;
    Sno<AmbientSound> x38_AmbientSoundSno;
    Sno<Reverb> x3C_ReverbSno;
    Sno<Weather> x40_WeatherSno;
    Sno<Worlds> x44_WorldsSno;
    int x48;
    int x4C;
    int x50;
    uint32 x54_;
    uint32 x58_;
    uint32 x5C_;
    uint32 x60_;
    uint32 x64_;
    uint32 x68_;
    uint32 x6C_;
    uint32 x70_;
    uint32 x74_;
    uint32 x78_;
    uint32 x7C_;
    uint32 x80_;
    uint32 x84_;
    int x88;
    SceneCachedValues x8C_SceneCachedValues;
    void dumpfunc() {
      dumpval(x00, x04_DT_IVECTOR2D, x0C_Snos, x1C_WorldsSno, x20, x24_LevelAreaSno);
      dumpval(x28_WorldsSno, x2C, x30_LevelAreaSno, x34_MusicSno, x38_AmbientSoundSno, x3C_ReverbSno);
      dumpval(x40_WeatherSno, x44_WorldsSno, x48, x4C, x50, x88);
      dumpval(x8C_SceneCachedValues);
    }
  };

  declstruct(SceneChunk) {
    SNOName x000_SNOName;
    PRTransform x008_PRTransform;
    SceneSpecification x024_SceneSpecification;
    void dumpfunc() {
      dumpval(x000_SNOName, x008_PRTransform, x024_SceneSpecification);
    }
  };

  declstruct(SceneParams) {
    SerializeData x00_SerializeData;
    int x08;
    uint32 x0C_;
    Array<SceneChunk> x10_SceneChunks = x00_SerializeData;
    void dumpfunc() {
      dumpval(x08, x10_SceneChunks);
    }
  };

  declstruct(WorldServerData) {
    SerializeData x00_SerializeData;
    Array<DRLGParams> x08_DRLGParams = x00_SerializeData;
    SerializeData x10_SerializeData;
    Array<SceneParams> x18_SceneParams = x10_SerializeData;
    LabelRuleSet x20_LabelRuleSet;
    int x38;
    uint32 x3C_;
    SceneClusterSet x40_SceneClusterSet;
    int x58_Snos[4];
    GameBalanceId x68_GameBalanceIds[3];
    int x74_35Sno;
    int x78;
    uint32 x7C_;
    Array<int> x80_Snos = x88_SerializeData;
    SerializeData x88_SerializeData;
    void dumpfunc() {
      dumpval(x08_DRLGParams, x18_SceneParams, x20_LabelRuleSet, x38, x40_SceneClusterSet, x58_Snos);
      dumpval(x68_GameBalanceIds, x74_35Sno, x78, x80_Snos);
    }
  };

  declstruct(DT_RGBACOLOR) {
    uint8 x00;
    uint8 x01;
    uint8 x02;
    uint8 x03;
    void dumpfunc() {
      dumpval(x00, x01, x02, x03);
    }
  };

  declstruct(PostFXParams) {
    float x00_float[4];
    float x10_float[4];
    void dumpfunc() {
      dumpval(x00_float, x10_float);
    }
  };

  declstruct(Environment) {
    DT_RGBACOLOR x00_DT_RGBACOLOR;
    uint8 x03_;
    PostFXParams x04_PostFXParams;
    Sno<Actor> x24_ActorSno;
    Sno<Music> x28_MusicSno;
    Sno<Music> x2C_MusicSno;
    float x30;
    float x34;
    Sno<AmbientSound> x38_AmbientSoundSno;
    Sno<Reverb> x3C_ReverbSno;
    Sno<Weather> x40_WeatherSno;
    Sno<Textures> x44_TexturesSno;
    Sno<Textures> x48_TexturesSno;
    void dumpfunc() {
      dumpval(x00_DT_RGBACOLOR, x04_PostFXParams, x24_ActorSno, x28_MusicSno, x2C_MusicSno, x30);
      dumpval(x34, x38_AmbientSoundSno, x3C_ReverbSno, x40_WeatherSno, x44_TexturesSno, x48_TexturesSno);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  SerializeData x10_SerializeData;
  Array<WorldServerData> x18_WorldServerDatas = x10_SerializeData;
  SerializeData x20_SerializeData;
  Array<int> x28_Snos = x20_SerializeData;
  uint32 x30_;
  uint32 x34_;
  uint32 x38_;
  uint32 x3C_;
  uint32 x40_;
  uint32 x44_;
  uint32 x48_;
  uint32 x4C_;
  uint32 x50_;
  uint32 x54_;
  uint32 x58_;
  uint32 x5C_;
  Environment x60_Environment;
  float xAC;
  int xB0;
  float xB4;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x18_WorldServerDatas, x28_Snos, x60_Environment, xAC);
    dumpval(xB0, xB4);
  }
};

#pragma pack(pop)
