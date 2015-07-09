#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(AnimTree::Type) {
  declstruct(AnimTreeLayerName) {
    int x00;
    void dumpfunc() {
      dumpval(x00);
    }
  };

  declstruct(AnimTreeLeaf) {
    int x00_Enum; // Animation = 0, PiecewiseLinearBlend = 1, BoneWeightedBlend = 2, SwitchBlend = 3, Pose = 4, AdditiveBlend = 5, BoneWeightsLinearBlend = 6, BoneWeightsMultiplyBlend = 7
    Sno<Anim> x04_AnimSno;
    int x08;
    float x0C;
    float x10;
    int x14;
    int x18_Enum; // Continuous = 0, InactiveReset = 1
    int x1C_Enum; // Action = 0, GetHit = 1, None = 16
    AnimTreeLayerName x20_AnimTreeLayerName;
    void dumpfunc() {
      dumpval(x00_Enum, x04_AnimSno, x08, x0C, x10, x14);
      dumpval(x18_Enum, x1C_Enum, x20_AnimTreeLayerName);
    }
  };

  declstruct(AnimTreeBlendCase) {
    int x00_Enum; // ForwardSpeed = 0, TurnSpeed = 1, IsIdle = 2, WeaponClass = 3, WalkSlowSpeed = 4, WalkSpeed = 5, RunSpeed = 6, SprintSpeed = 7, InTown = 8, IsSpecialMove = 9, AbsTurnSpeed = 10, AimYaw = 11, AimBlend = 12, IsAlive = 13, IsTurning = 14, None = 16
    int x04;
    float x08;
    void dumpfunc() {
      dumpval(x00_Enum, x04, x08);
    }
  };

  declstruct(AnimTreeBlendRamp) {
    float x00;
    float x04;
    float x08;
    float x0C;
    float x10;
    float x14;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
    }
  };

  declstruct(AnimTreeNode) {
    int x000_Enum; // Animation = 0, PiecewiseLinearBlend = 1, BoneWeightedBlend = 2, SwitchBlend = 3, Pose = 4, AdditiveBlend = 5, BoneWeightsLinearBlend = 6, BoneWeightsMultiplyBlend = 7
    int x004;
    int x008_int[16];
    int x048_Enum; // ForwardSpeed = 0, TurnSpeed = 1, IsIdle = 2, WeaponClass = 3, WalkSlowSpeed = 4, WalkSpeed = 5, RunSpeed = 6, SprintSpeed = 7, InTown = 8, IsSpecialMove = 9, AbsTurnSpeed = 10, AimYaw = 11, AimBlend = 12, IsAlive = 13, IsTurning = 14, None = 16
    AnimTreeBlendCase x04C_AnimTreeBlendCases[16];
    AnimTreeBlendRamp x10C_AnimTreeBlendRamp;
    int x124;
    AnimTreeLayerName x128_AnimTreeLayerName;
    void dumpfunc() {
      dumpval(x000_Enum, x004, x008_int, x048_Enum, x04C_AnimTreeBlendCases, x10C_AnimTreeBlendRamp);
      dumpval(x124, x128_AnimTreeLayerName);
    }
  };

  SnoHeader x00_Header;
  int x0C;
  int x10;
  int x14;
  SerializeData x18_SerializeData;
  Array<AnimTreeLeaf> x20_AnimTreeLeafs = x18_SerializeData;
  int x28;
  SerializeData x2C_SerializeData;
  uint32 x34_;
  Array<AnimTreeNode> x38_AnimTreeNodes = x2C_SerializeData;
  int x40;
  SerializeData x44_SerializeData;
  uint32 x4C_;
  Array<int> x50_int = x44_SerializeData;
  int x58;
  uint32 x5C_;
  void dumpfunc() {
    dumpval(x00_Header, x0C, x10, x14, x20_AnimTreeLeafs, x28);
    dumpval(x38_AnimTreeNodes, x40, x50_int, x58);
  }
};

#pragma pack(pop)
