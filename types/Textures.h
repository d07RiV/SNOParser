#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"
#include "image.h"

#pragma pack(push, 1)

declstruct(Textures::Type) {
  declstruct(TexFrame) {
    char name[64];
    float x0, y0, x1, y1;
    void dumpfunc() {
      dumpval(name, x0, y0, x1, y1);
    }
  };

  SnoHeader header;
  int flags, pixelFormat;
  int width, height;
  int mipLevels;
  int x020, x024;
  Binary data[60];
  int numFrames;
  SerializeData x20C_data;
  uint32 x214_;
  Array<TexFrame> frames = x20C_data;
  int vecX, vecY;
  int x228, x22C, x230, x234;
  uint64 x238;
  int x240;
  uint32 ptr1;
  int pool, x24C;
  Array<int> ids = x258_data;
  SerializeData x258_data;
  int x260, x264, x268, x26C;
  void dumpfunc() {
    dumpval(header, flags, pixelFormat, width, height, mipLevels);
    dumpval(x020, x024, data, numFrames, frames);
    dumpval(vecX, vecY, x228, x22C, x230, x234);
    dumpval(x238, x240, pool, x24C, ids);
    dumpval(x260, x264, x268);
  }

  Image load(int mip = 0) const;
};

#pragma pack(pop)
