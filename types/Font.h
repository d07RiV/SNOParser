#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Font::Type) {
  SnoHeader x00_Header;
  int x0C;
  char x10_Text[64];
  char x50_Text[64];
  int x90;
  int x94_Enum; // Disabled = 1, Native = 0, Auto = 2
  int x98_Enum; // Normal = 0, Light = 1, LCD = 2, LCD Vertical = 3
  int x9C;
  float xA0;
  float xA4;
  SerializeData xA8_SerializeData;
  Array<uint8> xB0_byte = xA8_SerializeData;
  int xB8;
  int xBC;
  int64 xC0;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10_Text, x50_Text, x90, x94_Enum);
    dumpval(x98_Enum, x9C, xA0, xA4, xB0_byte, xB8);
    dumpval(xBC, xC0);
  }
};

#pragma pack(pop)
