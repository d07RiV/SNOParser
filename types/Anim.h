#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Anim::Type) {
  declstruct(BoneName) {
    char x00_Text[64];
    void dumpfunc() {
      dumpval(x00_Text);
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

  declstruct(HardpointLink) {
    char x00_Text[64];
    int x40;
    void dumpfunc() {
      dumpval(x00_Text, x40);
    }
  };

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

  declstruct(LookLink) {
    char x00_Text[64];
    void dumpfunc() {
      dumpval(x00_Text);
    }
  };

  declstruct(TriggerConditions) {
    int x00_Percent;
    int x04_Time;
    int x08_Time;
    int x0C_Time;
    int x10_Time;
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
    int x184_Time;
    DT_RGBACOLOR x188_DT_RGBACOLOR;
    int x18C_Time;
    DT_RGBACOLOR x190_DT_RGBACOLOR;
    int x194_Time;
    void dumpfunc() {
      dumpval(x000, x004_TriggerConditions, x028, x02C_SNOName, x034, x038);
      dumpval(x03C, x040, x044_HardpointLinks, x0CC_LookLink, x10C_ConstraintLink, x14C);
      dumpval(x150, x154, x158, x15C, x160, x164);
      dumpval(x168, x16C, x170, x174, x178, x17C_Velocity);
      dumpval(x180, x184_Time, x188_DT_RGBACOLOR, x18C_Time, x190_DT_RGBACOLOR, x194_Time);
    }
  };

  declstruct(KeyframedAttachment) {
    float x000;
    TriggerEvent x004_TriggerEvent;
    void dumpfunc() {
      dumpval(x000, x004_TriggerEvent);
    }
  };

  declstruct(Quaternion16) {
    float16 x00;
    float16 x02;
    float16 x04;
    float16 x06;
    void dumpfunc() {
      dumpval(x00, x02, x04, x06);
    }
  };

  declstruct(RotationKey) {
    int x00;
    Quaternion16 x04_Quaternion16;
    void dumpfunc() {
      dumpval(x00, x04_Quaternion16);
    }
  };

  declstruct(RotationCurve) {
    int x00;
    SerializeData x04_SerializeData;
    int x0C;
    Array<RotationKey> x10_RotationKeies = x04_SerializeData;
    void dumpfunc() {
      dumpval(x00, x0C, x10_RotationKeies);
    }
  };

  declstruct(ScaleKey) {
    int x00;
    float x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  declstruct(ScaleCurve) {
    int x00;
    SerializeData x04_SerializeData;
    int x0C;
    Array<ScaleKey> x10_ScaleKeies = x04_SerializeData;
    void dumpfunc() {
      dumpval(x00, x0C, x10_ScaleKeies);
    }
  };

  declstruct(TranslationKey) {
    int x00;
    DT_VECTOR3D x04_DT_VECTOR3D;
    void dumpfunc() {
      dumpval(x00, x04_DT_VECTOR3D);
    }
  };

  declstruct(TranslationCurve) {
    int x00;
    SerializeData x04_SerializeData;
    int x0C;
    Array<TranslationKey> x10_TranslationKeies = x04_SerializeData;
    void dumpfunc() {
      dumpval(x00, x0C, x10_TranslationKeies);
    }
  };

  declstruct(VelocityVector3D) {
    float x00_Velocity;
    float x04_Velocity;
    float x08_Velocity;
    void dumpfunc() {
      dumpval(x00_Velocity, x04_Velocity, x08_Velocity);
    }
  };

  declstruct(AnimPermutation) {
    int x000;
    char x004_Text[68];
    float x048_Velocity;
    float x04C;
    float x050;
    float x054;
    float x058;
    int x05C_Time;
    int x060_Time;
    int x064;
    float x068;
    float x06C;
    float x070;
    float x074;
    int x078;
    SerializeData x07C_SerializeData;
    uint32 x084_;
    Array<BoneName> x088_BoneNames = x07C_SerializeData;
    int x090;
    SerializeData x094_SerializeData;
    uint32 x09C_;
    Array<TranslationCurve> x0A0_TranslationCurves = x094_SerializeData;
    SerializeData x0A8_SerializeData;
    Array<RotationCurve> x0B0_RotationCurves = x0A8_SerializeData;
    SerializeData x0B8_SerializeData;
    Array<ScaleCurve> x0C0_ScaleCurves = x0B8_SerializeData;
    float x0C8;
    float x0CC;
    float x0D0;
    float x0D4;
    DT_VECTOR3D x0D8_DT_VECTOR3D;
    DT_VECTOR3D x0E4_DT_VECTOR3D;
    DT_VECTOR3D x0F0_DT_VECTOR3D;
    DT_VECTOR3D x0FC_DT_VECTOR3D;
    float x108;
    SerializeData x10C_SerializeData;
    int x114;
    Array<KeyframedAttachment> x118_KeyframedAttachments = x10C_SerializeData;
    SerializeData x120_SerializeData;
    Array<DT_VECTOR3D> x128_DT_VECTOR3Ds = x120_SerializeData;
    SerializeData x130_SerializeData;
    Array<DT_VECTOR3D> x138_DT_VECTOR3Ds = x130_SerializeData;
    VelocityVector3D x140_VelocityVector3D;
    HardpointLink x14C_HardpointLink;
    uint32 x190_;
    uint32 x194_;
    void dumpfunc() {
      dumpval(x000, x004_Text, x048_Velocity, x04C, x050, x054);
      dumpval(x058, x05C_Time, x060_Time, x064, x068, x06C);
      dumpval(x070, x074, x078, x088_BoneNames, x090, x0A0_TranslationCurves);
      dumpval(x0B0_RotationCurves, x0C0_ScaleCurves, x0C8, x0CC, x0D0, x0D4);
      dumpval(x0D8_DT_VECTOR3D, x0E4_DT_VECTOR3D, x0F0_DT_VECTOR3D, x0FC_DT_VECTOR3D, x108, x114);
      dumpval(x118_KeyframedAttachments, x128_DT_VECTOR3Ds, x138_DT_VECTOR3Ds, x140_VelocityVector3D, x14C_HardpointLink);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  int x10;
  Sno<Appearance> x14_AppearanceSno;
  SerializeData x18_SerializeData;
  int x20;
  uint32 x24_;
  Array<AnimPermutation> x28_AnimPermutations = x18_SerializeData;
  int x30;
  uint32 x34_;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x14_AppearanceSno, x20, x28_AnimPermutations);
    dumpval(x30);
  }
};

#pragma pack(pop)

std::pair<uint32, std::vector<float>> GetFPA(char const* name);
std::map<uint32, std::vector<float>> const& BaseFPA();