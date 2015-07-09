#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(UI::Type) {
  declstruct(UIAnimationDescription) {
    int64 x000;
    char x008_Text[512];
    int x208;
    int x20C_int[2];
    uint32 x214_;
    void dumpfunc() {
      dumpval(x000, x008_Text, x208, x20C_int);
    }
  };

  declstruct(UIAnimationElementFloat) {
    float x00;
    float x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  declstruct(UIAnimationElementInt32) {
    float x00;
    int x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  declstruct(UIAnimationElementVector2D) {
    float x00;
    float x04_float[2];
    void dumpfunc() {
      dumpval(x00, x04_float);
    }
  };

  declstruct(UIControlAnimationBinding) {
    char x000_Text[512];
    int x200;
    float x204;
    void dumpfunc() {
      dumpval(x000_Text, x200, x204);
    }
  };

  declstruct(UIAnimationElements) {
    int x00;
    SerializeData x04_SerializeData;
    uint32 x0C_;
    Array<UIAnimationElementInt32> x10_UIAnimationElementInt32s = x04_SerializeData;
    int x18;
    SerializeData x1C_SerializeData;
    uint32 x24_;
    Array<UIAnimationElementFloat> x28_UIAnimationElementFloats = x1C_SerializeData;
    int x30;
    SerializeData x34_SerializeData;
    uint32 x3C_;
    Array<UIAnimationElementVector2D> x40_UIAnimationElementVector2Ds = x34_SerializeData;
    int x48;
    SerializeData x4C_SerializeData;
    uint32 x54_;
    Array<UIAnimationDescription> x58_UIAnimationDescriptions = x4C_SerializeData;
    int x60;
    SerializeData x64_SerializeData;
    uint32 x6C_;
    Array<UIControlAnimationBinding> x70_UIControlAnimationBindings = x64_SerializeData;
    void dumpfunc() {
      dumpval(x00, x10_UIAnimationElementInt32s, x18, x28_UIAnimationElementFloats, x30, x40_UIAnimationElementVector2Ds);
      dumpval(x48, x58_UIAnimationDescriptions, x60, x70_UIControlAnimationBindings);
    }
  };

  declstruct(UIControlElement) {
    int64 x000;
    char x008_Text[256];
    void dumpfunc() {
      dumpval(x000, x008_Text);
    }
  };

  declstruct(UIDataElementFloat32) {
    int64 x00;
    float x08;
    uint32 x0C_;
    void dumpfunc() {
      dumpval(x00, x08);
    }
  };

  declstruct(UIDataElementInt32) {
    int64 x00;
    int x08;
    uint32 x0C_;
    void dumpfunc() {
      dumpval(x00, x08);
    }
  };

  declstruct(DataElements) {
    int x00;
    SerializeData x04_SerializeData;
    uint32 x0C_;
    Array<UIControlElement> x10_UIControlElements = x04_SerializeData;
    int x18;
    SerializeData x1C_SerializeData;
    uint32 x24_;
    Array<UIDataElementInt32> x28_UIDataElementInt32s = x1C_SerializeData;
    int x30;
    SerializeData x34_SerializeData;
    uint32 x3C_;
    Array<UIDataElementFloat32> x40_UIDataElementFloat32s = x34_SerializeData;
    void dumpfunc() {
      dumpval(x00, x10_UIControlElements, x18, x28_UIDataElementInt32s, x30, x40_UIDataElementFloat32s);
    }
  };

  declstruct(UIStyleChildSpec) {
    char x000_Text[512];
    DataElements x200_DataElements;
    uint32 x248_;
    uint32 x24C_;
    uint32 x250_;
    uint32 x254_;
    uint32 x258_;
    uint32 x25C_;
    void dumpfunc() {
      dumpval(x000_Text, x200_DataElements);
    }
  };

  declstruct(UIStyle) {
    int64 x000;
    char x008_Text[512];
    DataElements x208_DataElements;
    int x250;
    SerializeData x254_SerializeData;
    uint32 x25C_;
    Array<UIStyleChildSpec> x260_UIStyleChildSpecs = x254_SerializeData;
    uint32 x268_;
    uint32 x26C_;
    uint32 x270_;
    uint32 x274_;
    uint32 x278_;
    uint32 x27C_;
    uint32 x280_;
    uint32 x284_;
    uint32 x288_;
    uint32 x28C_;
    void dumpfunc() {
      dumpval(x000, x008_Text, x208_DataElements, x250, x260_UIStyleChildSpecs);
    }
  };

  declstruct(UIControlData) {
    int x000;
    uint32 x004_;
    int64 x008;
    char x010_Text[512];
    int64 x210;
    char x218_Text[512];
    int x418;
    uint32 x41C_;
    UIStyle x420_UIStyle;
    UIAnimationElements x6B0_UIAnimationElements;
    uint32 x728_;
    uint32 x72C_;
    uint32 x730_;
    uint32 x734_;
    uint32 x738_;
    uint32 x73C_;
    void dumpfunc() {
      dumpval(x000, x008, x010_Text, x210, x218_Text, x418);
      dumpval(x420_UIStyle, x6B0_UIAnimationElements);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  int x10;
  SerializeData x14_SerializeData;
  uint32 x1C_;
  Array<UIControlData> x20_UIControlDatas = x14_SerializeData;
  int x28;
  SerializeData x2C_SerializeData;
  uint32 x34_;
  Array<UIStyle> x38_UIStyles = x2C_SerializeData;
  DataElements x40_DataElements;
  uint32 x88_;
  uint32 x8C_;
  uint32 x90_;
  uint32 x94_;
  uint32 x98_;
  uint32 x9C_;
  uint32 xA0_;
  uint32 xA4_;
  uint32 xA8_;
  uint32 xAC_;
  uint32 xB0_;
  uint32 xB4_;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x20_UIControlDatas, x28, x38_UIStyles);
    dumpval(x40_DataElements);
  }
};

#pragma pack(pop)
