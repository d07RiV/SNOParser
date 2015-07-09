#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(EffectGroup::Type) {
  declstruct(ConstraintLink) {
    char x00_Text[64];
    void dumpfunc() {
      dumpval(x00_Text);
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

  declstruct(HardpointLink) {
    char x00_Text[64];
    int x40;
    void dumpfunc() {
      dumpval(x00_Text, x40);
    }
  };

  declstruct(LookLink) {
    char x00_Text[64];
    void dumpfunc() {
      dumpval(x00_Text);
    }
  };

  declstruct(SNOName) {
    int x00_SnoGroup;
    int x04_SnoNameHandle;
    void dumpfunc() {
      dumpval(x00_SnoGroup, x04_SnoNameHandle);
    }
  };

  declstruct(TriggerConditions) {
    int x00_Percent;
    float x04_Time;
    float x08_Time;
    float x0C_Time;
    float x10_Time;
    float x14_Impulse;
    float x18_Impulse;
    int x1C;
    int x20;
    void dumpfunc() {
      dumpval(x00_Percent, x04_Time, x08_Time, x0C_Time, x10_Time, x14_Impulse);
      dumpval(x18_Impulse, x1C, x20);
    }
  };

  declstruct(TriggerEvent) {
    int x000;
    TriggerConditions x004_TriggerConditions;
    int x028;
    SNOName x02C_SNOName;
    int x034;
    int x038;
    int x03C;
    int x040;
    HardpointLink x044_HardpointLinks[2];
    LookLink x0CC_LookLink;
    ConstraintLink x10C_ConstraintLink;
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
    float x17C_Velocity;
    int x180;
    float x184_Time;
    DT_RGBACOLOR x188_DT_RGBACOLOR;
    uint8 x18B_;
    float x18C_Time;
    DT_RGBACOLOR x190_DT_RGBACOLOR;
    uint8 x193_;
    float x194_Time;
    void dumpfunc() {
      dumpval(x000, x004_TriggerConditions, x028, x02C_SNOName, x034, x038);
      dumpval(x03C, x040, x044_HardpointLinks, x0CC_LookLink, x10C_ConstraintLink, x14C);
      dumpval(x150, x154, x158, x15C, x160, x164);
      dumpval(x168, x16C, x170, x174, x178, x17C_Velocity);
      dumpval(x180, x184_Time, x188_DT_RGBACOLOR, x18C_Time, x190_DT_RGBACOLOR, x194_Time);
    }
  };

  declstruct(MsgTriggeredEvent) {
    int x000;
    TriggerEvent x004_TriggerEvent;
    void dumpfunc() {
      dumpval(x000, x004_TriggerEvent);
    }
  };

  declstruct(EffectItem) {
    int x000;
    char x004_Text[64];
    MsgTriggeredEvent x044_MsgTriggeredEvent;
    void dumpfunc() {
      dumpval(x000, x004_Text, x044_MsgTriggeredEvent);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  SerializeData x10_SerializeData;
  int x18;
  uint32 x1C_;
  Array<EffectItem> x20_EffectItems = x10_SerializeData;
  int x28;
  int x2C;
  int x30;
  Sno<Power> x34_PowerSno;
  int x38_int[16];
  void dumpfunc() {
    dumpval(x00_Header, x0C, x18, x20_EffectItems, x28, x2C);
    dumpval(x30, x34_PowerSno, x38_int);
  }
};

#pragma pack(pop)
