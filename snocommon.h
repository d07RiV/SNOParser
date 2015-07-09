#pragma once

#include "serialize.h"

#pragma pack(push, 1)

declstruct(SnoHeader) {
  int id, lock, flags;
  void dumpfunc() {
    dumpval(id, lock, flags);
  }
};

declstruct(Vector) {
  float x, y, z;
  void dumpfunc() {
    dumpval(x, y, z);
  }
};

declstruct(Quaternion) {
  short x, y, z, w;
  void dumpfunc() {
    dumpval(x, y, z, w);
  }
};

declstruct(SNOName) {
  int group;
  int handle;
  void dumpfunc() {
    dumpval(group, handle);
  }
};

declstruct(Color) {
  uint8 r, g, b, a;
  void dumpfunc() {
    dumpval(r, g, b, a);
  }
};


declstruct(TriggerConditions) {
  int x00_chance;
  int x04_time;
  int x08_time;
  int x0C_time;
  int x10_time;
  float x14_impulse;
  float x18_impulse;
  int x1C;
  int x20;
  void dumpfunc() {
    dumpval(x00_chance, x04_time, x08_time, x0C_time);
    dumpval(x10_time, x14_impulse, x18_impulse);
    dumpval(x1C, x20);
  }
};

declstruct(HardpointLink) {
  char x00_text[64];
  int x40;
  void dumpfunc() {
    dumpval(x00_text, x40);
  }
};

declstruct(TriggerEvent) {
  int x000_type;
  TriggerConditions x004_conditions;
  int x028;
  SNOName x02C_snoName;
  int x034;
  int x038;
  int x03C;
  int x040;
  HardpointLink x044_link;
  HardpointLink x088_link;
  char x0CC_look[64];
  char x10C_constraint[64];
  int x14C;
  float x150;
  int x154;
  int x158;
  int x15C;
  int x160;
  int x164;
  float x168;
  float x16C;
  int x170;
  float x174;
  int x178;
  float x17C;
  int x180;
  float x184;
  Color x188_color;
  float x18C;
  Color x190_color;
  float x194;
  void dumpfunc() {
    dumpval(x000_type, x004_conditions, x028, x02C_snoName);
    dumpval(x034, x038, x03C, x040);
    dumpval(x044_link, x088_link, x0CC_look);
    dumpval(x10C_constraint, x14C, x150, x154);
    dumpval(x158, x15C, x160, x164);
    dumpval(x168, x16C, x170, x174);
    dumpval(x178, x17C, x180, x184);
    dumpval(x188_color, x18C, x190_color, x194);
  }
};

#pragma pack(pop)

