#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Globals::Type) {
  declstruct(ActorGroup) {
    int x00;
    char x04_Text[64];
    void dumpfunc() {
      dumpval(x00, x04_Text);
    }
  };

  declstruct(DifficultyTuningParams) {
    float x00;
    float x04;
    float x08;
    float x0C;
    float x10;
    float x14;
    float x18;
    float x1C;
    float x20;
    float x24;
    float x28;
    float x2C;
    float x30;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
      dumpval(x18, x1C, x20, x24, x28, x2C);
      dumpval(x30);
    }
  };

  declstruct(GlobalScriptVariable) {
    int x00;
    char x04_Text[32];
    float x24;
    void dumpfunc() {
      dumpval(x00, x04_Text, x24);
    }
  };

  declstruct(GlobalServerData) {
    Array<ActorGroup> x000_ActorGroups = x008_SerializeData;
    SerializeData x008_SerializeData;
    Array<GlobalScriptVariable> x010_GlobalScriptVariables = x018_SerializeData;
    SerializeData x018_SerializeData;
    DifficultyTuningParams x020_DifficultyTuningParams[6];
    float x158;
    float x15C;
    float x160;
    float x164;
    float x168;
    float x16C;
    float x170;
    float x174;
    int x178;
    float x17C;
    float x180;
    float x184;
    float x188;
    float x18C;
    float x190;
    float x194;
    float x198;
    float x19C;
    float x1A0;
    int x1A4;
    int x1A8;
    int x1AC;
    float x1B0;
    float x1B4;
    float x1B8;
    float x1BC;
    float x1C0;
    float x1C4;
    float x1C8;
    int x1CC;
    float x1D0;
    int x1D4;
    int x1D8;
    int x1DC;
    int x1E0;
    int x1E4;
    int x1E8;
    int x1EC;
    int x1F0;
    int x1F4;
    float x1F8;
    float x1FC;
    float x200_float[10];
    float x228;
    float x22C;
    int x230;
    int x234;
    int x238;
    float x23C;
    int x240;
    float x244;
    float x248;
    float x24C;
    float x250;
    float x254;
    float x258;
    float x25C;
    float x260;
    float x264;
    float x268;
    float x26C_float[2];
    float x274;
    float x278;
    float x27C;
    float x280;
    float x284;
    float x288;
    float x28C;
    float x290;
    int x294;
    int x298;
    int x29C;
    int x2A0;
    int x2A4;
    int x2A8;
    float x2AC;
    float x2B0;
    float x2B4;
    float x2B8_float[6];
    float x2D0;
    int x2D4;
    int x2D8;
    float x2DC;
    void dumpfunc() {
      dumpval(x000_ActorGroups, x010_GlobalScriptVariables, x020_DifficultyTuningParams, x158, x15C, x160);
      dumpval(x164, x168, x16C, x170, x174, x178);
      dumpval(x17C, x180, x184, x188, x18C, x190);
      dumpval(x194, x198, x19C, x1A0, x1A4, x1A8);
      dumpval(x1AC, x1B0, x1B4, x1B8, x1BC, x1C0);
      dumpval(x1C4, x1C8, x1CC, x1D0, x1D4, x1D8);
      dumpval(x1DC, x1E0, x1E4, x1E8, x1EC, x1F0);
      dumpval(x1F4, x1F8, x1FC, x200_float, x228, x22C);
      dumpval(x230, x234, x238, x23C, x240, x244);
      dumpval(x248, x24C, x250, x254, x258, x25C);
      dumpval(x260, x264, x268, x26C_float, x274, x278);
      dumpval(x27C, x280, x284, x288, x28C, x290);
      dumpval(x294, x298, x29C, x2A0, x2A4, x2A8);
      dumpval(x2AC, x2B0, x2B4, x2B8_float, x2D0, x2D4);
      dumpval(x2D8, x2DC);
    }
  };

  declstruct(StartLocationName) {
    int x00;
    char x04_Text[64];
    void dumpfunc() {
      dumpval(x00, x04_Text);
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

  declstruct(BannerColorSet) {
    DT_RGBACOLOR x00_DT_RGBACOLORs[2];
    uint8 x06_;
    uint8 x07_;
    char x08_Text[64];
    int x48;
    int x4C;
    uint32 x50_;
    void dumpfunc() {
      dumpval(x00_DT_RGBACOLORs, x08_Text, x48, x4C);
    }
  };

  declstruct(BannerSigilPlacement) {
    char x00_Text[64];
    int x40;
    void dumpfunc() {
      dumpval(x00_Text, x40);
    }
  };

  declstruct(BannerTexturePair) {
    Sno<Textures> x00_TexturesSno;
    int x04;
    int x08;
    uint32 x0C_;
    void dumpfunc() {
      dumpval(x00_TexturesSno, x04, x08);
    }
  };

  declstruct(EpicBannerDescription) {
    Sno<Textures> x00_TexturesSno;
    Sno<Actor> x04_ActorSno;
    Sno<Actor> x08_ActorSno;
    int x0C;
    char x10_Text[128];
    uint32 x90_;
    void dumpfunc() {
      dumpval(x00_TexturesSno, x04_ActorSno, x08_ActorSno, x0C, x10_Text);
    }
  };

  declstruct(BannerParams) {
    Array<BannerTexturePair> x00_BannerTexturePairs = x08_SerializeData;
    SerializeData x08_SerializeData;
    int x10;
    uint32 x14_;
    Array<BannerTexturePair> x18_BannerTexturePairs = x20_SerializeData;
    SerializeData x20_SerializeData;
    int x28;
    uint32 x2C_;
    Array<BannerTexturePair> x30_BannerTexturePairs = x38_SerializeData;
    SerializeData x38_SerializeData;
    Array<BannerTexturePair> x40_BannerTexturePairs = x48_SerializeData;
    SerializeData x48_SerializeData;
    int x50;
    uint32 x54_;
    Array<BannerTexturePair> x58_BannerTexturePairs = x60_SerializeData;
    SerializeData x60_SerializeData;
    int x68;
    uint32 x6C_;
    Array<BannerColorSet> x70_BannerColorSets = x78_SerializeData;
    SerializeData x78_SerializeData;
    Array<BannerSigilPlacement> x80_BannerSigilPlacements = x88_SerializeData;
    SerializeData x88_SerializeData;
    Array<int> x90_Snos = x98_SerializeData;
    SerializeData x98_SerializeData;
    Array<int> xA0_Snos = xA8_SerializeData;
    SerializeData xA8_SerializeData;
    Array<int> xB0_Snos = xB8_SerializeData;
    SerializeData xB8_SerializeData;
    Array<int> xC0_Snos = xC8_SerializeData;
    SerializeData xC8_SerializeData;
    Array<EpicBannerDescription> xD0_EpicBannerDescriptions = xD8_SerializeData;
    SerializeData xD8_SerializeData;
    uint32 xE0_;
    uint32 xE4_;
    uint32 xE8_;
    uint32 xEC_;
    void dumpfunc() {
      dumpval(x00_BannerTexturePairs, x10, x18_BannerTexturePairs, x28, x30_BannerTexturePairs, x40_BannerTexturePairs);
      dumpval(x50, x58_BannerTexturePairs, x68, x70_BannerColorSets, x80_BannerSigilPlacements, x90_Snos);
      dumpval(xA0_Snos, xB0_Snos, xC0_Snos, xD0_EpicBannerDescriptions);
    }
  };

  declstruct(AssetListEntry) {
    int x00;
    int x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  declstruct(AssetList) {
    Array<AssetListEntry> x00_AssetListEntries = x08_SerializeData;
    SerializeData x08_SerializeData;
    void dumpfunc() {
      dumpval(x00_AssetListEntries);
    }
  };

  SnoHeader x000_Header;
  uint32 x00C_;
  Array<GlobalServerData> x010_GlobalServerDatas = x018_SerializeData;
  SerializeData x018_SerializeData;
  int x020;
  uint32 x024_;
  Array<StartLocationName> x028_StartLocationNames = x030_SerializeData;
  SerializeData x030_SerializeData;
  float x038;
  float x03C;
  float x040;
  float x044;
  int x048;
  int x04C;
  float x050;
  float x054;
  int x058;
  float x05C;
  float x060;
  float x064;
  float x068;
  float x06C;
  float x070;
  float x074;
  float x078;
  float x07C;
  int x080;
  int x084_int[4];
  uint32 x094_;
  BannerParams x098_BannerParams;
  int x188;
  int x18C;
  int x190;
  int x194;
  float x198;
  int x19C;
  float x1A0;
  float x1A4;
  float x1A8;
  float x1AC;
  float x1B0;
  float x1B4;
  float x1B8;
  float x1BC;
  float x1C0;
  float x1C4;
  float x1C8;
  float x1CC;
  float x1D0;
  float x1D4;
  float x1D8;
  float x1DC;
  float x1E0;
  float x1E4;
  float x1E8;
  float x1EC;
  float x1F0;
  int x1F4;
  float x1F8;
  int x1FC;
  int x200;
  float x204;
  float x208;
  float x20C;
  float x210;
  float x214;
  float x218;
  float x21C;
  AssetList x220_AssetLists[8];
  float x2A0;
  float x2A4;
  void dumpfunc() {
    dumpval(x000_Header, x010_GlobalServerDatas, x020, x028_StartLocationNames, x038, x03C);
    dumpval(x040, x044, x048, x04C, x050, x054);
    dumpval(x058, x05C, x060, x064, x068, x06C);
    dumpval(x070, x074, x078, x07C, x080, x084_int);
    dumpval(x098_BannerParams, x188, x18C, x190, x194, x198);
    dumpval(x19C, x1A0, x1A4, x1A8, x1AC, x1B0);
    dumpval(x1B4, x1B8, x1BC, x1C0, x1C4, x1C8);
    dumpval(x1CC, x1D0, x1D4, x1D8, x1DC, x1E0);
    dumpval(x1E4, x1E8, x1EC, x1F0, x1F4, x1F8);
    dumpval(x1FC, x200, x204, x208, x20C, x210);
    dumpval(x214, x218, x21C, x220_AssetLists, x2A0, x2A4);
  }
};

#pragma pack(pop)
