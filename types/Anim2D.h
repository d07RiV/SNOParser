#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Anim2D::Type) {
  declstruct(FrameAnim) {
    int x00;
    float x04_Velocity;
    float x08_Velocity;
    int x0C;
    int x10;
    void dumpfunc() {
      dumpval(x00, x04_Velocity, x08_Velocity, x0C, x10);
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

  declstruct(Anim2DFrame) {
    char x00_Text[64];
    DT_RGBACOLOR x40_DT_RGBACOLOR;
    uint8 x43_;
    void dumpfunc() {
      dumpval(x00_Text, x40_DT_RGBACOLOR);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  int x10;
  FrameAnim x14_FrameAnim;
  Sno<Sound> x28_SoundSno;
  int x2C;
  SerializeData x30_SerializeData;
  Array<Anim2DFrame> x38_Anim2DFrames = x30_SerializeData;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x14_FrameAnim, x28_SoundSno, x2C);
    dumpval(x38_Anim2DFrames);
  }
};

#pragma pack(pop)
