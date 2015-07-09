#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Shaders::Type) {
  declstruct(DT_RGBACOLOR) {
    uint8 x00;
    uint8 x01;
    uint8 x02;
    uint8 x03;
    void dumpfunc() {
      dumpval(x00, x01, x02, x03);
    }
  };

  declstruct(RenderParams) {
    int x00;
    int x04;
    int x08;
    float x0C;
    float x10;
    int x14;
    int x18;
    int x1C;
    int x20;
    int x24;
    int x28;
    int x2C;
    int x30;
    uint8 x34;
    uint8 x35_;
    uint8 x36_;
    uint8 x37_;
    int x38;
    int x3C;
    int x40;
    int x44;
    int x48;
    int x4C;
    int x50;
    int x54;
    int x58;
    DT_RGBACOLOR x5C_DT_RGBACOLOR;
    uint8 x5F_;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
      dumpval(x18, x1C, x20, x24, x28, x2C);
      dumpval(x30, x34, x38, x3C, x40, x44);
      dumpval(x48, x4C, x50, x54, x58, x5C_DT_RGBACOLOR);
    }
  };

  declstruct(TextureStageParams) {
    int x00;
    int x04;
    int x08;
    int x0C;
    int x10;
    float x14;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
    }
  };

  declstruct(RenderPass) {
    int x000;
    int x004;
    RenderParams x008_RenderParams;
    int x068;
    int x06C;
    SerializeData x070_SerializeData;
    Array<TextureStageParams> x078_TextureStageParams = x070_SerializeData;
    int x080;
    char x084_Text[256];
    char x184_Text[64];
    char x1C4_Text[64];
    SerializeData x204_SerializeData;
    uint32 x20C_;
    Array<int> x210_TagMap = x204_SerializeData; // type: 0x00100000
    uint32 x218_;
    uint32 x21C_;
    uint32 x220_;
    uint32 x224_;
    uint32 x228_;
    uint32 x22C_;
    int x230;
    int x234;
    void dumpfunc() {
      dumpval(x000, x004, x008_RenderParams, x068, x06C, x078_TextureStageParams);
      dumpval(x080, x084_Text, x184_Text, x1C4_Text, x210_TagMap, x230);
      dumpval(x234);
    }
  };

  SnoHeader x000_Header;
  int x00C;
  int x010;
  int x014;
  SerializeData x018_SerializeData;
  Array<RenderPass> x020_RenderPass = x018_SerializeData;
  char x028_Text[256];
  void dumpfunc() {
    dumpval(x000_Header, x00C, x010, x014, x020_RenderPass, x028_Text);
  }
};

#pragma pack(pop)
