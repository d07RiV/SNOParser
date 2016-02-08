#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(GameBalance::Type) {
  declstruct(AttributeSpecifier) {
    int x00_Type; // TranslateFromString = 0x0110FAC0, TranslateFromValue = 0x0110FB70
    int x04_Param;
    Array<TagValue> x08_Data = x10_SerializeData;
    SerializeData x10_SerializeData;
    void dumpfunc() {
      dumpval(x00_Type, x04_Param, x08_Data);
    }
  };

  declstruct(ItemType) {
    char x000_Text[256];
    int x100;
    int x104;
    GameBalanceId x108_ItemTypesGameBalanceId;
    int x10C;
    int x110;
    int x114;
    uint32 x118_Bit0 : 1, x118_Bit1 : 1, : 1, x118_Bit3 : 1, : 2, x118_Bit6 : 1, x118_Bit7 : 1, x118_Bit8 : 1, x118_Bit9 : 1, x118_Bit10 : 1, x118_Bit11 : 1, x118_Bit12 : 1, x118_Bit13 : 1, x118_Bit14 : 1, x118_Bit15 : 1, x118_Bit16 : 1, x118_Bit17 : 1;
    int x11C_Enum; // Player:Backpack = 0, Player:Head = 1, Player:Torso = 2, Player:RightHand = 3, Player:LeftHand = 4, Player:Hands = 5, Player:Waist = 6, Player:Feet = 7, Player:Shoulders = 8, Player:Legs = 9, Player:Bracers = 10, Player:LeftFinger = 12, Player:RightFinger = 11, Player:Neck = 13, Merchant = 18, Pet:RightHand = 20, Pet:LeftHand = 21, Pet:Special = 22, Pet:LeftFinger = 25, Pet:RightFinger = 24, Pet:Neck = 23
    int x120_Enum; // Player:Backpack = 0, Player:Head = 1, Player:Torso = 2, Player:RightHand = 3, Player:LeftHand = 4, Player:Hands = 5, Player:Waist = 6, Player:Feet = 7, Player:Shoulders = 8, Player:Legs = 9, Player:Bracers = 10, Player:LeftFinger = 12, Player:RightFinger = 11, Player:Neck = 13, Merchant = 18, Pet:RightHand = 20, Pet:LeftHand = 21, Pet:Special = 22, Pet:LeftFinger = 25, Pet:RightFinger = 24, Pet:Neck = 23
    int x124_Enum; // Player:Backpack = 0, Player:Head = 1, Player:Torso = 2, Player:RightHand = 3, Player:LeftHand = 4, Player:Hands = 5, Player:Waist = 6, Player:Feet = 7, Player:Shoulders = 8, Player:Legs = 9, Player:Bracers = 10, Player:LeftFinger = 12, Player:RightFinger = 11, Player:Neck = 13, Merchant = 18, Pet:RightHand = 20, Pet:LeftHand = 21, Pet:Special = 22, Pet:LeftFinger = 25, Pet:RightFinger = 24, Pet:Neck = 23
    int x128_Enum; // Player:Backpack = 0, Player:Head = 1, Player:Torso = 2, Player:RightHand = 3, Player:LeftHand = 4, Player:Hands = 5, Player:Waist = 6, Player:Feet = 7, Player:Shoulders = 8, Player:Legs = 9, Player:Bracers = 10, Player:LeftFinger = 12, Player:RightFinger = 11, Player:Neck = 13, Merchant = 18, Pet:RightHand = 20, Pet:LeftHand = 21, Pet:Special = 22, Pet:LeftFinger = 25, Pet:RightFinger = 24, Pet:Neck = 23
    GameBalanceId x12C_AffixListGameBalanceId;
    GameBalanceId x130_AffixListGameBalanceId;
    GameBalanceId x134_AffixListGameBalanceId;
    GameBalanceId x138_AffixGroupGameBalanceId;
    int x13C_int[5];
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108_ItemTypesGameBalanceId, x10C, x110);
      dumpval(x114, x118_Bit0, x118_Bit1, x118_Bit3, x118_Bit6, x118_Bit7);
      dumpval(x118_Bit8, x118_Bit9, x118_Bit10, x118_Bit11, x118_Bit12, x118_Bit13);
      dumpval(x118_Bit14, x118_Bit15, x118_Bit16, x118_Bit17, x11C_Enum, x120_Enum);
      dumpval(x124_Enum, x128_Enum, x12C_AffixListGameBalanceId, x130_AffixListGameBalanceId, x134_AffixListGameBalanceId, x138_AffixGroupGameBalanceId);
      dumpval(x13C_int);
    }
  };

  declstruct(RecipeIngredient) {
    GameBalanceId x00_ItemsGameBalanceId;
    int x04;
    void dumpfunc() {
      dumpval(x00_ItemsGameBalanceId, x04);
    }
  };

  declstruct(Item) {
    char x000_Text[256];
    int x100;
    int x104;
    Sno<Actor> x108_ActorSno;
    GameBalanceId x10C_ItemTypesGameBalanceId;
    uint32 x110_Bit0 : 1, x110_Bit1 : 1, x110_Bit2 : 1, x110_Bit3 : 1, x110_Bit4 : 1, x110_Bit5 : 1, x110_Bit6 : 1, x110_Bit7 : 1, x110_Bit8 : 1, x110_Bit9 : 1, x110_Bit10 : 1, x110_Bit11 : 1, x110_Bit12 : 1, x110_Bit13 : 1, x110_Bit14 : 1, x110_Bit15 : 1, : 1, x110_Bit17 : 1, x110_Bit18 : 1, x110_Bit19 : 1, x110_Bit20 : 1, x110_Bit21 : 1, x110_Bit22 : 1, x110_Bit23 : 1, x110_Bit24 : 1;
    int x114;
    int x118;
    int x11C_Enum; // Invalid = -1, A1 = 0, A2 = 100, A3 = 200, A4 = 300, A5 = 400, OpenWorld = 3000, WeeklyChallenge = 4000, Test = 1000
    int x120;
    int x124;
    int x128;
    int x12C;
    int x130;
    int x134;
    int x138;
    int x13C;
    int x140;
    int x144;
    int x148;
    int x14C;
    int x150;
    int x154;
    int x158;
    int x15C;
    int x160;
    int x164;
    int x168;
    GameBalanceId x16C_ItemsGameBalanceId;
    GameBalanceId x170_SetItemBonusesGameBalanceId;
    int x174_TreasureClassSno;
    int x178_TreasureClassSno;
    int x17C_TreasureClassSno;
    int x180_TreasureClassSno;
    Sno<StringList> x184_StringListSno;
    Sno<StringList> x188_StringListSno;
    Sno<EffectGroup> x18C_EffectGroupSno;
    Sno<EffectGroup> x190_EffectGroupSno;
    int x194;
    int x198;
    int x19C;
    int x1A0;
    int x1A4_int[5];
    int x1B8;
    float x1BC;
    float x1C0;
    float x1C4;
    float x1C8;
    float x1CC;
    float x1D0;
    float x1D4;
    Sno<Power> x1D8_PowerSno;
    int x1DC;
    Sno<Power> x1E0_PowerSno;
    int x1E4;
    Sno<Power> x1E8_PowerSno;
    int x1EC;
    Sno<Power> x1F0_PowerSno;
    int x1F4;
    AttributeSpecifier x1F8_AttributeSpecifiers[16];
    int x378_Enum; // Invalid = -1, Inferior = 0, Normal = 1, Superior = 2, Magic1 = 3, Magic2 = 4, Magic3 = 5, Rare4 = 6, Rare5 = 7, Rare6 = 8, Legendary = 9, Special = 10, Set = 11
    GameBalanceId x37C_GameBalanceIds[10];
    GameBalanceId x3A4_GameBalanceIds[8];
    int x3C4;
    int x3C8;
    int x3CC;
    int x3D0;
    int x3D4;
    int x3D8;
    int x3DC;
    int x3E0;
    GameBalanceId x3E4_AffixGroupGameBalanceId;
    GameBalanceId x3E8_AffixGroupGameBalanceId;
    GameBalanceId x3EC_AffixGroupGameBalanceId;
    GameBalanceId x3F0_AffixGroupGameBalanceId;
    GameBalanceId x3F4_AffixGroupGameBalanceId;
    GameBalanceId x3F8_AffixGroupGameBalanceId;
    int x3FC;
    int x400;
    int x404;
    int x408;
    int x40C;
    int x410;
    int x414;
    int x418;
    int x41C;
    int x420;
    int x424;
    int x428;
    GameBalanceId x42C_GameBalanceId;
    int x430_Enum; // Amethyst = 1, Emerald = 2, Ruby = 3, Topaz = 4, Diamond = 5
    int x434;
    int x438_Enum; // A = 1, B = 2, C = 3, D = 4
    Sno<Actor> x43C_ActorSno;
    Sno<Worlds> x440_WorldsSno;
    Sno<Worlds> x444_WorldsSno;
    Sno<LevelArea> x448_LevelAreaSno;
    int x44C;
    RecipeIngredient x450_RecipeIngredients[6];
    int x480;
    RecipeIngredient x484_RecipeIngredients[6];
    int x4B4;
    int x4B8_;
    int x4BC_;
    AttributeSpecifier x4B8_AttributeSpecifier;
    AttributeSpecifier x4D0_AttributeSpecifier;
    int x4E8;
    int x4EC;
    Sno<EffectGroup> x4F0_EffectGroupSno;
    int x4F4;
    int x4F8;
    int x4FC;
    int x508_;
    int x50C_;
    int x510_;
    int x514_;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108_ActorSno, x10C_ItemTypesGameBalanceId, x110_Bit0);
      dumpval(x110_Bit1, x110_Bit2, x110_Bit3, x110_Bit4, x110_Bit5, x110_Bit6);
      dumpval(x110_Bit7, x110_Bit8, x110_Bit9, x110_Bit10, x110_Bit11, x110_Bit12);
      dumpval(x110_Bit13, x110_Bit14, x110_Bit15, x110_Bit17, x110_Bit18, x110_Bit19);
      dumpval(x110_Bit20, x110_Bit21, x110_Bit22, x110_Bit23, x110_Bit24, x114);
      dumpval(x118, x11C_Enum, x120, x124, x128, x12C);
      dumpval(x130, x134, x138, x13C, x140, x144);
      dumpval(x148, x14C, x150, x154, x158, x15C);
      dumpval(x160, x164, x168, x16C_ItemsGameBalanceId, x170_SetItemBonusesGameBalanceId, x174_TreasureClassSno);
      dumpval(x178_TreasureClassSno, x17C_TreasureClassSno, x180_TreasureClassSno, x184_StringListSno, x188_StringListSno, x18C_EffectGroupSno);
      dumpval(x190_EffectGroupSno, x194, x198, x19C, x1A0, x1A4_int);
      dumpval(x1B8, x1BC, x1C0, x1C4, x1C8, x1CC);
      dumpval(x1D0, x1D4, x1D8_PowerSno, x1DC, x1E0_PowerSno, x1E4);
      dumpval(x1E8_PowerSno, x1EC, x1F0_PowerSno, x1F4);
      dumpval(x1F8_AttributeSpecifiers);
      dumpval(x378_Enum, x37C_GameBalanceIds);
      dumpval(x3A4_GameBalanceIds, x3C4, x3C8, x3CC, x3D0, x3D4);
      dumpval(x3D8, x3DC, x3E0, x3E4_AffixGroupGameBalanceId, x3E8_AffixGroupGameBalanceId, x3EC_AffixGroupGameBalanceId);
      dumpval(x3F0_AffixGroupGameBalanceId, x3F4_AffixGroupGameBalanceId, x3F8_AffixGroupGameBalanceId, x3FC, x400, x404);
      dumpval(x408, x40C, x410, x414, x418, x41C);
      dumpval(x420, x424, x428, x42C_GameBalanceId, x430_Enum, x434);
      dumpval(x438_Enum, x43C_ActorSno, x440_WorldsSno, x444_WorldsSno, x448_LevelAreaSno, x44C);
      dumpval(x450_RecipeIngredients, x480, x484_RecipeIngredients, x4B4);
      dumpval(x4B8_AttributeSpecifier, x4D0_AttributeSpecifier, x4E8, x4EC, x4F0_EffectGroupSno, x4F4);
      dumpval(x4F8, x4FC);
    }
  };

  declstruct(ExperienceLevel) {
    int64 x000;
    int x008;
    int x00C;
    float x010;
    float x014;
    int x018;
    int x01C;
    int x020;
    int x024;
    int x028;
    int x02C;
    int x030;
    int x034;
    int x038;
    int x03C;
    int x040;
    int x044;
    int x048;
    int x04C;
    int x050;
    int x054;
    int x058;
    int x05C;
    int x060;
    int x064;
    int x068;
    int x06C;
    int x070;
    int x074;
    int x078;
    int x07C;
    int x080;
    int x084;
    int x088;
    int x08C;
    int x090;
    int x094;
    int x098;
    int x09C;
    int x0A0;
    int x0A4;
    int x0A8;
    float x0AC;
    float x0B0;
    float x0B4;
    int x0B8;
    int x0BC;
    int x0C0;
    int x0C4;
    int x0C8;
    int x0CC;
    int x0D0;
    int x0D4;
    int x0D8;
    int x0DC;
    int x0E0;
    int x0E4;
    int x0E8;
    int x0EC;
    int x0F0;
    int x0F4;
    int x0F8;
    int x0FC;
    int x100;
    int x104;
    int x108;
    int x10C;
    int x110;
    int x114;
    uint32 x118_;
    uint32 x11C_;
    uint32 x120_;
    uint32 x124_;
    int x128;
    int x12C;
    int x130;
    int x134;
    int x138;
    int x13C;
    int x140;
    int x144;
    int x148;
    int x14C;
    int x150;
    int x154;
    uint32 x158_;
    float x15C;
    float x160;
    float x164;
    int64 x168;
    int x170;
    int x174;
    int x178;
    int x17C;
    int x180;
    int x184;
    int x188;
    int x18C;
    int x190;
    int x194;
    int x198;
    int x19C;
    int x1A0;
    int x1A4;
    int x1A8;
    int x1AC;
    int x1B0;
    int x1B4;
    float x1B8;
    int x1BC;
    int x1C0;
    int x1C4;
    int x1C8;
    int x1CC;
    int x1D0;
    uint32 x1D4_;
    void dumpfunc() {
      dumpval(x000, x008, x00C, x010, x014, x018);
      dumpval(x01C, x020, x024, x028, x02C, x030);
      dumpval(x034, x038, x03C, x040, x044, x048);
      dumpval(x04C, x050, x054, x058, x05C, x060);
      dumpval(x064, x068, x06C, x070, x074, x078);
      dumpval(x07C, x080, x084, x088, x08C, x090);
      dumpval(x094, x098, x09C, x0A0, x0A4, x0A8);
      dumpval(x0AC, x0B0, x0B4, x0B8, x0BC, x0C0);
      dumpval(x0C4, x0C8, x0CC, x0D0, x0D4, x0D8);
      dumpval(x0DC, x0E0, x0E4, x0E8, x0EC, x0F0);
      dumpval(x0F4, x0F8, x0FC, x100, x104, x108);
      dumpval(x10C, x110, x114, x128, x12C, x130);
      dumpval(x134, x138, x13C, x140, x144, x148);
      dumpval(x14C, x150, x154, x15C, x160, x164);
      dumpval(x168, x170, x174, x178, x17C, x180);
      dumpval(x184, x188, x18C, x190, x194, x198);
      dumpval(x19C, x1A0, x1A4, x1A8, x1AC, x1B0);
      dumpval(x1B4, x1B8, x1BC, x1C0, x1C4, x1C8);
      dumpval(x1CC, x1D0);
    }
  };

  declstruct(ExperienceLevelAlt) {
    int64 x00;
    int x08;
    int x0C;
    int x10;
    int x14;
    int x18;
    int x1C;
    int x20;
    int x24;
    int x28;
    int x2C;
    int x30;
    int x34;
    int x38;
    int x3C;
    int x40;
    int x44;
    int x48;
    int x4C;
    int x50;
    int x54;
    int x58;
    int x5C;
    int x60;
    int x64;
    int x68;
    int x6C;
    void dumpfunc() {
      dumpval(x00, x08, x0C, x10, x14, x18);
      dumpval(x1C, x20, x24, x28, x2C, x30);
      dumpval(x34, x38, x3C, x40, x44, x48);
      dumpval(x4C, x50, x54, x58, x5C, x60);
      dumpval(x64, x68, x6C);
    }
  };

  declstruct(HelpCodes) {
    SerializeData x00_SerializeData;
    Array<HelpCodes> x08_HelpCodes = x00_SerializeData;
    void dumpfunc() {
      dumpval(x08_HelpCodes);
    }
  };

  declstruct(MonsterLevelDefinition) {
    int x00;
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
    float x34;
    float x38;
    float x3C;
    float x40;
    float x44;
    float x48;
    float x4C;
    float x50;
    float x54;
    float x58;
    float x5C;
    float x60;
    float x64;
    float x68;
    float x6C;
    float x70;
    float x74;
    float x78;
    float x7C;
    float x80;
    float x84;
    float x88;
    float x8C;
    float x90;
    float x94;
    float x98;
    float x9C;
    float xA0;
    float xA4;
    float xA8;
    float xAC;
    float xB0;
    float xB4;
    float xB8;
    float xBC;
    float xC0;
    float xC4;
    float xC8;
    float xCC;
    float xD0;
    float xD4;
    float xD8;
    float xDC;
    float xE0;
    float xE4;
    float xE8;
    int xEC;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
      dumpval(x18, x1C, x20, x24, x28, x2C);
      dumpval(x30, x34, x38, x3C, x40, x44);
      dumpval(x48, x4C, x50, x54, x58, x5C);
      dumpval(x60, x64, x68, x6C, x70, x74);
      dumpval(x78, x7C, x80, x84, x88, x8C);
      dumpval(x90, x94, x98, x9C, xA0, xA4);
      dumpval(xA8, xAC, xB0, xB4, xB8, xBC);
      dumpval(xC0, xC4, xC8, xCC, xD0, xD4);
      dumpval(xD8, xDC, xE0, xE4, xE8, xEC);
    }
  };

  declstruct(AffixTableEntry) {
    char x000_Text[256];
    int x100;
    int x104;
    int x108;
    int x10C;
    int x110;
    int x114;
    int x118;
    int x11C;
    int x120;
    int x124;
    int x128;
    int x12C;
    int x130;
    int x134;
    int x138;
    int x13C;
    int x140;
    int x144;
    int x148;
    int x14C;
    int x150_Enum; // - None - = 0, Lightning = 1, Cold = 2, Fire = 3, Poison = 4, Arcane = 5, WitchdoctorDamage = 6, LifeSteal = 7, ManaSteal = 8, MagicFind = 9, GoldFind = 10, AttackSpeedBonus = 11, CastSpeedBonus = 12, Holy = 13, WizardDamage = 14
    int x154;
    GameBalanceId x158_AffixListGameBalanceId;
    GameBalanceId x15C_AffixListGameBalanceId;
    Sno<StringList> x160_StringListSno;
    Sno<StringList> x164_StringListSno;
    GameBalanceId x168_AffixGroupGameBalanceId;
    GameBalanceId x16C_AffixGroupGameBalanceId;
    int x170_Enum; // - None - = -1, DemonHunter = 0, Barbarian = 1, Wizard = 2, Witchdoctor = 3, Monk = 4, Crusader = 5
    GameBalanceId x174_AffixListGameBalanceId;
    GameBalanceId x178_GameBalanceIds[6];
    GameBalanceId x190_GameBalanceIds[24];
    GameBalanceId x1F0_GameBalanceIds[24];
    int x250;
    int x254_Enum; // Prefix = 0, Suffix = 1, Inherent = 2, Title = 5, Quality = 6, Immunity = 7, Random = 9, Enchantment = 10, Socket Enhancement = 11
    GameBalanceId x258_AffixListGameBalanceId;
    uint32 x25C_;
    AttributeSpecifier x260_AttributeSpecifiers[4];
    uint32 x2C0_;
    uint32 x2C4_;
    uint32 x2C8_;
    uint32 x2CC_;
    uint32 x2D0_;
    uint32 x2D4_;
    uint32 x2D8_;
    uint32 x2DC_;
    uint32 x2E0_;
    uint32 x2E4_;
    uint32 x2E8_;
    uint32 x2EC_;
    uint32 x2F0_;
    uint32 x2F4_;
    uint32 x2F8_;
    uint32 x2FC_;
    uint32 x300_;
    uint32 x304_;
    int x308_Enum; // Major = 0, Minor = 1
    uint32 x30C_;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108, x10C, x110);
      dumpval(x114, x118, x11C, x120, x124, x128);
      dumpval(x12C, x130, x134, x138, x13C, x140);
      dumpval(x144, x148, x14C, x150_Enum, x154, x158_AffixListGameBalanceId);
      dumpval(x15C_AffixListGameBalanceId, x160_StringListSno, x164_StringListSno, x168_AffixGroupGameBalanceId, x16C_AffixGroupGameBalanceId, x170_Enum);
      dumpval(x174_AffixListGameBalanceId, x178_GameBalanceIds, x190_GameBalanceIds, x1F0_GameBalanceIds, x250, x254_Enum);
      dumpval(x258_AffixListGameBalanceId, x260_AttributeSpecifiers, x308_Enum);
    }
  };

  declstruct(HeroData) {
    char x000_Text[256];
    int x100;
    int x104;
    Sno<Actor> x108_ActorSno;
    Sno<Actor> x10C_ActorSno;
    int x110_TreasureClassSno;
    int x114;
    Sno<Power> x118_PowerSno;
    Sno<Power> x11C_PowerSno;
    Sno<SkillKit> x120_SkillKitSno;
    Sno<SkillKit> x124_SkillKitSno;
    Sno<SkillKit> x128_SkillKitSno;
    Sno<SkillKit> x12C_SkillKitSno;
    int x130_Enum; // Mana = 0, Arcanum = 1, Fury = 2, Spirit = 3, Power = 4, Hatred = 5, Discipline = 6, Faith = 7
    int x134_Enum; // Mana = 0, Arcanum = 1, Fury = 2, Spirit = 3, Power = 4, Hatred = 5, Discipline = 6, Faith = 7
    int x138_Enum; // None = -1, Strength = 0, Dexterity = 1, Intelligence = 2
    float x13C;
    int x140;
    float x144;
    float x148;
    float x14C;
    float x150;
    float x154;
    float x158;
    float x15C;
    float x160;
    float x164;
    float x168;
    float x16C;
    float x170;
    float x174;
    float x178;
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
    float x1F4;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108_ActorSno, x10C_ActorSno, x110_TreasureClassSno);
      dumpval(x114, x118_PowerSno, x11C_PowerSno, x120_SkillKitSno, x124_SkillKitSno, x128_SkillKitSno);
      dumpval(x12C_SkillKitSno, x130_Enum, x134_Enum, x138_Enum, x13C, x140);
      dumpval(x144, x148, x14C, x150, x154, x158);
      dumpval(x15C, x160, x164, x168, x16C, x170);
      dumpval(x174, x178, x17C, x180, x184, x188);
      dumpval(x18C, x190, x194, x198, x19C, x1A0);
      dumpval(x1A4, x1A8, x1AC, x1B0, x1B4, x1B8);
      dumpval(x1BC, x1C0, x1C4, x1C8, x1CC, x1D0);
      dumpval(x1D4, x1D8, x1DC, x1E0, x1E4, x1E8);
      dumpval(x1EC, x1F0, x1F4);
    }
  };

  declstruct(MovementStyleDefinition) {
    char x000_Text[256];
    int x100;
    int x104;
    int x108;
    int x10C;
    int x110;
    int x114;
    int x118;
    int x11C;
    int x120;
    int x124;
    float x128;
    float x12C;
    float x130;
    float x134;
    float x138;
    float x13C;
    float x140;
    float x144;
    float x148;
    float x14C;
    float x150;
    float x154;
    float x158;
    float x15C;
    float x160;
    float x164;
    float x168;
    float x16C;
    float x170;
    float x174;
    float x178;
    float x17C;
    Sno<Power> x180_PowerSno;
    int x184;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108, x10C, x110);
      dumpval(x114, x118, x11C, x120, x124, x128);
      dumpval(x12C, x130, x134, x138, x13C, x140);
      dumpval(x144, x148, x14C, x150, x154, x158);
      dumpval(x15C, x160, x164, x168, x16C, x170);
      dumpval(x174, x178, x17C, x180_PowerSno, x184);
    }
  };

  declstruct(LabelGBID) {
    char x000_Text[256];
    int x100;
    int x104;
    int x108;
    int x10C;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108, x10C);
    }
  };

  declstruct(LootDistribution) {
    int x00;
    int x04;
    int x08;
    int x0C;
    int x10;
    int x14;
    int x18;
    int x1C;
    int x20;
    int x24;
    float x28;
    float x2C;
    float x30;
    float x34;
    float x38;
    float x3C;
    float x40;
    float x44;
    float x48;
    float x4C;
    float x50;
    int x54;
    int x58;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
      dumpval(x18, x1C, x20, x24, x28, x2C);
      dumpval(x30, x34, x38, x3C, x40, x44);
      dumpval(x48, x4C, x50, x54, x58);
    }
  };

  declstruct(RareItemNamesEntry) {
    char x000_Text[256];
    int x100;
    int x104;
    int x108_Enum; // Item Types = 1, Items = 2, Experience Table = 3, Help Codes = 24, Monster Levels = 5, Heros = 7, Affix List = 8, Movement Styles = 10, Labels = 11, Loot Distribution = 12, Rare Item Names = 16, Monster Affixes = 18, Monster Names = 19, Socketed Effects = 21, Item Drop Table = 25, Item Level Modifiers = 26, Quality Classes = 27, Handicaps = 28, Item Salvage Levels = 29, Scenery = 17, Hirelings = 4, Set Item Bonuses = 33, Elite Modifiers = 34, Item Tiers = 35, Power Formula Tables = 36, Recipes = 32, Scripted Achievement Events = 37, LootRun Quest Tiers = 39, Paragon Bonuses = 40, Legacy Item Conversions = 45, Enchant Item Affix Use Count Cost Scalars = 46, Tiered Loot Run Levels = 49, Transmute Recipes = 50
    GameBalanceId x10C_GameBalanceId;
    int x110_Enum; // Prefix = 0, Suffix = 1, Inherent = 2, Title = 5, Quality = 6, Immunity = 7, Random = 9, Enchantment = 10, Socket Enhancement = 11
    int x114;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108_Enum, x10C_GameBalanceId, x110_Enum);
      dumpval(x114);
    }
  };

  declstruct(MonsterAffixesEntry) {
    char x000_Text[256];
    int x100;
    int x104;
    int x108;
    int x10C;
    int x110;
    int x114_Enum; // All = 0, Rares = 1, Shooters = 2, Champions = 3
    int x118_Enum; // None = -1, Physical = 0, Fire = 1, Lightning = 2, Cold = 3, Poison = 4, Arcane = 5, Holy = 6
    int x11C_Enum; // Prefix = 0, Suffix = 1, Inherent = 2, Title = 5, Quality = 6, Immunity = 7, Random = 9, Enchantment = 10, Socket Enhancement = 11
    int x120;
    int x124;
    int x128;
    int x12C;
    AttributeSpecifier x130_Attributes[20];
    uint32 x310_;
    Sno<Power> x314_PowerSno;
    Sno<Power> x318_PowerSno;
    Sno<Power> x31C_PowerSno;
    uint8 x320;
    uint8 x321;
    uint8 x322;
    uint8 x323;
    uint8 x324;
    uint8 x325;
    uint8 x326;
    uint8 x327;
    uint8 x328;
    uint8 x329;
    uint8 x32A;
    uint8 x32B;
    uint8 x32C;
    uint8 x32D;
    uint8 x32E;
    uint8 x32F;
    uint8 x330;
    uint8 x331;
    uint8 x332;
    uint8 x333;
    uint8 x334;
    uint8 x335;
    uint8 x336;
    uint8 x337;
    uint8 x338;
    uint8 x339;
    uint8 x33A;
    uint8 x33B;
    uint8 x33C;
    uint8 x33D;
    uint8 x33E;
    uint8 x33F;
    uint8 x340;
    uint8 x341;
    uint8 x342;
    uint8 x343;
    uint8 x344;
    uint8 x345;
    uint8 x346;
    uint8 x347;
    uint8 x348;
    uint8 x349;
    uint8 x34A;
    uint8 x34B;
    uint8 x34C;
    uint8 x34D;
    uint8 x34E;
    uint8 x34F;
    uint8 x350;
    uint8 x351;
    uint8 x352;
    uint8 x353;
    uint8 x354;
    uint8 x355;
    uint8 x356;
    uint8 x357;
    uint8 x358;
    uint8 x359;
    uint8 x35A;
    uint8 x35B;
    uint8 x35C;
    uint8 x35D;
    uint8 x35E;
    uint8 x35F;
    uint8 x360;
    uint8 x361;
    uint8 x362;
    uint8 x363;
    uint8 x364;
    uint8 x365;
    uint8 x366;
    uint8 x367;
    uint8 x368;
    uint8 x369;
    uint8 x36A;
    uint8 x36B;
    uint8 x36C;
    uint8 x36D;
    uint8 x36E;
    uint8 x36F;
    uint8 x370;
    uint8 x371;
    uint8 x372;
    uint8 x373;
    uint8 x374;
    uint8 x375;
    uint8 x376;
    uint8 x377;
    uint8 x378;
    uint8 x379;
    uint8 x37A;
    uint8 x37B;
    uint8 x37C;
    uint8 x37D;
    uint8 x37E;
    uint8 x37F;
    uint8 x380;
    uint8 x381;
    uint8 x382;
    uint32 x383_;
    uint8 x387_;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108, x10C, x110);
      dumpval(x114_Enum, x118_Enum, x11C_Enum, x120, x124, x128);
      dumpval(x12C, x130_Attributes);
      dumpval(x314_PowerSno, x318_PowerSno, x31C_PowerSno, x320, x321);
      dumpval(x322, x323, x324, x325, x326, x327);
      dumpval(x328, x329, x32A, x32B, x32C, x32D);
      dumpval(x32E, x32F, x330, x331, x332, x333);
      dumpval(x334, x335, x336, x337, x338, x339);
      dumpval(x33A, x33B, x33C, x33D, x33E, x33F);
      dumpval(x340, x341, x342, x343, x344, x345);
      dumpval(x346, x347, x348, x349, x34A, x34B);
      dumpval(x34C, x34D, x34E, x34F, x350, x351);
      dumpval(x352, x353, x354, x355, x356, x357);
      dumpval(x358, x359, x35A, x35B, x35C, x35D);
      dumpval(x35E, x35F, x360, x361, x362, x363);
      dumpval(x364, x365, x366, x367, x368, x369);
      dumpval(x36A, x36B, x36C, x36D, x36E, x36F);
      dumpval(x370, x371, x372, x373, x374, x375);
      dumpval(x376, x377, x378, x379, x37A, x37B);
      dumpval(x37C, x37D, x37E, x37F, x380, x381);
      dumpval(x382);
    }
  };

  declstruct(RareMonsterNamesEntry) {
    char x000_Text[256];
    int x100;
    int x104;
    int x108_Enum; // Prefix = 0, Suffix = 1, Inherent = 2, Title = 5, Quality = 6, Immunity = 7, Random = 9, Enchantment = 10, Socket Enhancement = 11
    char x10C_Text[128];
    int x18C;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108_Enum, x10C_Text, x18C);
    }
  };

  declstruct(SocketedEffectsTableEntry) {
    char x000_Text[256];
    int x100;
    int x104;
    GameBalanceId x108_ItemsGameBalanceId;
    GameBalanceId x10C_ItemTypesGameBalanceId;
    int x110_Translateable; // TranslateFromString = 0x0110FAC0, TranslateFromValue = 0x0110FB70
    int x114_AttributeParameter;
    Text x118_Text = x120_SerializeData;
    SerializeData x120_SerializeData;
    int x128_Translateable; // TranslateFromString = 0x0110FAC0, TranslateFromValue = 0x0110FB70
    int x12C_AttributeParameter;
    Text x130_Text = x138_SerializeData;
    SerializeData x138_SerializeData;
    int x140_Translateable; // TranslateFromString = 0x0110FAC0, TranslateFromValue = 0x0110FB70
    int x144_AttributeParameter;
    Text x148_Text = x150_SerializeData;
    SerializeData x150_SerializeData;
    int x158_Translateable; // TranslateFromString = 0x0110FAC0, TranslateFromValue = 0x0110FB70
    int x15C_AttributeParameter;
    Text x160_Text = x168_SerializeData;
    SerializeData x168_SerializeData;
    int x170_Translateable; // TranslateFromString = 0x0110FAC0, TranslateFromValue = 0x0110FB70
    int x174_AttributeParameter;
    Text x178_Text = x180_SerializeData;
    SerializeData x180_SerializeData;
    char x188_Text[1024];
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108_ItemsGameBalanceId, x10C_ItemTypesGameBalanceId, x110_Translateable);
      dumpval(x114_AttributeParameter, x118_Text, x128_Translateable, x12C_AttributeParameter, x130_Text, x140_Translateable);
      dumpval(x144_AttributeParameter, x148_Text, x158_Translateable, x15C_AttributeParameter, x160_Text, x170_Translateable);
      dumpval(x174_AttributeParameter, x178_Text, x188_Text);
    }
  };

  declstruct(ItemDropTableEntry) {
    char x000_Text[256];
    int x100;
    int x104;
    int x108;
    int x10C;
    int x110;
    int x114;
    int x118;
    int x11C;
    int x120;
    int x124;
    int x128;
    int x12C;
    int x130;
    int x134;
    int x138;
    int x13C;
    int x140;
    int x144;
    int x148;
    int x14C;
    int x150;
    int x154;
    int x158;
    int x15C;
    int x160;
    int x164;
    int x168;
    int x16C;
    int x170;
    int x174;
    int x178;
    int x17C;
    int x180;
    int x184;
    int x188;
    int x18C;
    int x190;
    int x194;
    int x198;
    int x19C;
    int x1A0;
    int x1A4;
    int x1A8;
    int x1AC;
    int x1B0;
    int x1B4;
    int x1B8;
    int x1BC;
    int x1C0;
    int x1C4;
    int x1C8;
    int x1CC;
    int x1D0;
    int x1D4;
    int x1D8;
    int x1DC;
    int x1E0;
    int x1E4;
    int x1E8;
    int x1EC;
    int x1F0;
    int x1F4;
    int x1F8;
    int x1FC;
    int x200;
    int x204;
    int x208;
    int x20C;
    int x210;
    int x214;
    int x218;
    int x21C;
    int x220;
    int x224;
    int x228;
    int x22C;
    int x230;
    int x234;
    int x238;
    int x23C;
    int x240;
    int x244;
    int x248;
    int x24C;
    int x250;
    int x254;
    int x258;
    int x25C;
    int x260;
    int x264;
    int x268;
    int x26C;
    int x270;
    int x274;
    int x278;
    int x27C;
    int x280;
    int x284;
    int x288;
    int x28C;
    int x290;
    int x294;
    int x298;
    int x29C;
    int x2A0;
    int x2A4;
    int x2A8;
    int x2AC;
    int x2B0;
    int x2B4;
    int x2B8;
    int x2BC;
    int x2C0;
    int x2C4;
    int x2C8;
    int x2CC;
    int x2D0;
    int x2D4;
    int x2D8;
    int x2DC;
    int x2E0;
    int x2E4;
    int x2E8;
    int x2EC;
    int x2F0;
    int x2F4;
    int x2F8;
    int x2FC;
    int x300;
    int x304;
    int x308;
    int x30C;
    int x310;
    int x314;
    int x318;
    int x31C;
    int x320;
    int x324;
    int x328;
    int x32C;
    int x330;
    int x334;
    int x338;
    int x33C;
    int x340;
    int x344;
    int x348;
    int x34C;
    int x350;
    int x354;
    int x358;
    int x35C;
    int x360;
    int x364;
    int x368;
    int x36C;
    int x370;
    int x374;
    int x378;
    int x37C;
    int x380;
    int x384;
    int x388;
    int x38C;
    int x390;
    int x394;
    int x398;
    int x39C;
    int x3A0;
    int x3A4;
    int x3A8;
    int x3AC;
    int x3B0;
    int x3B4;
    int x3B8;
    int x3BC;
    int x3C0;
    int x3C4;
    int x3C8;
    int x3CC;
    int x3D0;
    int x3D4;
    int x3D8;
    int x3DC;
    int x3E0;
    int x3E4;
    int x3E8;
    int x3EC;
    int x3F0;
    int x3F4;
    int x3F8;
    int x3FC;
    uint32 x400_;
    int x404;
    int x408;
    int x40C;
    int x410;
    int x414;
    int x418;
    int x41C;
    int x420;
    int x424;
    int x428;
    int x42C;
    int x430;
    int x434;
    int x438;
    int x43C;
    int x440;
    int x444;
    int x448;
    int x44C;
    int x450;
    int x454;
    int x458;
    int x45C;
    int x460;
    int x464;
    int x468;
    int x46C;
    int x470;
    int x474;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108, x10C, x110);
      dumpval(x114, x118, x11C, x120, x124, x128);
      dumpval(x12C, x130, x134, x138, x13C, x140);
      dumpval(x144, x148, x14C, x150, x154, x158);
      dumpval(x15C, x160, x164, x168, x16C, x170);
      dumpval(x174, x178, x17C, x180, x184, x188);
      dumpval(x18C, x190, x194, x198, x19C, x1A0);
      dumpval(x1A4, x1A8, x1AC, x1B0, x1B4, x1B8);
      dumpval(x1BC, x1C0, x1C4, x1C8, x1CC, x1D0);
      dumpval(x1D4, x1D8, x1DC, x1E0, x1E4, x1E8);
      dumpval(x1EC, x1F0, x1F4, x1F8, x1FC, x200);
      dumpval(x204, x208, x20C, x210, x214, x218);
      dumpval(x21C, x220, x224, x228, x22C, x230);
      dumpval(x234, x238, x23C, x240, x244, x248);
      dumpval(x24C, x250, x254, x258, x25C, x260);
      dumpval(x264, x268, x26C, x270, x274, x278);
      dumpval(x27C, x280, x284, x288, x28C, x290);
      dumpval(x294, x298, x29C, x2A0, x2A4, x2A8);
      dumpval(x2AC, x2B0, x2B4, x2B8, x2BC, x2C0);
      dumpval(x2C4, x2C8, x2CC, x2D0, x2D4, x2D8);
      dumpval(x2DC, x2E0, x2E4, x2E8, x2EC, x2F0);
      dumpval(x2F4, x2F8, x2FC, x300, x304, x308);
      dumpval(x30C, x310, x314, x318, x31C, x320);
      dumpval(x324, x328, x32C, x330, x334, x338);
      dumpval(x33C, x340, x344, x348, x34C, x350);
      dumpval(x354, x358, x35C, x360, x364, x368);
      dumpval(x36C, x370, x374, x378, x37C, x380);
      dumpval(x384, x388, x38C, x390, x394, x398);
      dumpval(x39C, x3A0, x3A4, x3A8, x3AC, x3B0);
      dumpval(x3B4, x3B8, x3BC, x3C0, x3C4, x3C8);
      dumpval(x3CC, x3D0, x3D4, x3D8, x3DC, x3E0);
      dumpval(x3E4, x3E8, x3EC, x3F0, x3F4, x3F8);
      dumpval(x3FC, x404, x408, x40C, x410, x414);
      dumpval(x418, x41C, x420, x424, x428, x42C);
      dumpval(x430, x434, x438, x43C, x440, x444);
      dumpval(x448, x44C, x450, x454, x458, x45C);
      dumpval(x460, x464, x468, x46C, x470, x474);
    }
  };

  declstruct(QualityClass) {
    char x000_Text[256];
    int x100;
    int x104;
    uint32 x108_;
    float x10C;
    float x110;
    float x114;
    float x118;
    float x11C;
    float x120;
    float x124;
    float x128;
    float x12C;
    float x130;
    float x134;
    float x138;
    float x13C;
    float x140;
    float x144;
    float x148;
    float x14C;
    float x150;
    float x154;
    float x158;
    float x15C;
    float x160;
    int x164;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x10C, x110, x114);
      dumpval(x118, x11C, x120, x124, x128, x12C);
      dumpval(x130, x134, x138, x13C, x140, x144);
      dumpval(x148, x14C, x150, x154, x158, x15C);
      dumpval(x160, x164);
    }
  };

  declstruct(HandicapLevel) {
    float x00;
    float x04;
    float x08;
    float x0C;
    float x10;
    float x14;
    int x18;
    int x1C;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
      dumpval(x18, x1C);
    }
  };

  declstruct(ItemSalvageLevel) {
    int x00_TreasureClassSno;
    int x04_TreasureClassSno;
    int x08_TreasureClassSno;
    int x0C_TreasureClassSno;
    void dumpfunc() {
      dumpval(x00_TreasureClassSno, x04_TreasureClassSno, x08_TreasureClassSno, x0C_TreasureClassSno);
    }
  };

  declstruct(HirelingEntry) {
    char x000_Text[256];
    int x100;
    int x104;
    Sno<Actor> x108_ActorSno;
    Sno<Actor> x10C_ActorSno;
    Sno<Actor> x110_ActorSno;
    int x114_TreasureClassSno;
    int x118_Enum; // None = -1, Strength = 0, Dexterity = 1, Intelligence = 2
    float x11C;
    float x120;
    float x124;
    float x128;
    float x12C;
    float x130;
    float x134;
    float x138;
    float x13C;
    float x140;
    float x144;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108_ActorSno, x10C_ActorSno, x110_ActorSno);
      dumpval(x114_TreasureClassSno, x118_Enum, x11C, x120, x124, x128);
      dumpval(x12C, x130, x134, x138, x13C, x140);
      dumpval(x144);
    }
  };

  declstruct(SetItemBonusTableEntry) {
    char x000_Text[256];
    int x100;
    int x104;
    GameBalanceId x108_SetItemBonusesGameBalanceId;
    int x10C;
    AttributeSpecifier x110_AttributeSpecifiers[8];
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108_SetItemBonusesGameBalanceId, x10C, x110_AttributeSpecifiers);
    }
  };

  declstruct(EliteModifierData) {
    char x000_Text[256];
    int x100;
    int x104;
    float x108;
    float x10C_Time;
    float x110;
    float x114_Time;
    float x118;
    float x11C_Time;
    float x120;
    float x124_Time;
    float x128;
    float x12C_Time;
    float x130;
    float x134_Time;
    float x138;
    float x13C_Time;
    float x140;
    float x144;
    float x148_Time;
    float x14C;
    float x150;
    float x154;
    float x158;
    int x15C;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108, x10C_Time, x110);
      dumpval(x114_Time, x118, x11C_Time, x120, x124_Time, x128);
      dumpval(x12C_Time, x130, x134_Time, x138, x13C_Time, x140);
      dumpval(x144, x148_Time, x14C, x150, x154, x158);
      dumpval(x15C);
    }
  };

  declstruct(ItemTierData) {
    GameBalanceId x00_ItemsGameBalanceId;
    GameBalanceId x04_ItemsGameBalanceId;
    GameBalanceId x08_ItemsGameBalanceId;
    GameBalanceId x0C_ItemsGameBalanceId;
    GameBalanceId x10_ItemsGameBalanceId;
    GameBalanceId x14_ItemsGameBalanceId;
    GameBalanceId x18_ItemsGameBalanceId;
    GameBalanceId x1C_ItemsGameBalanceId;
    void dumpfunc() {
      dumpval(x00_ItemsGameBalanceId, x04_ItemsGameBalanceId, x08_ItemsGameBalanceId, x0C_ItemsGameBalanceId, x10_ItemsGameBalanceId, x14_ItemsGameBalanceId);
      dumpval(x18_ItemsGameBalanceId, x1C_ItemsGameBalanceId);
    }
  };

  declstruct(PowerFormulaTableEntry) {
    char x000_Text[1024];
    float x400;
    float x404;
    float x408;
    float x40C;
    float x410;
    float x414;
    float x418;
    float x41C;
    float x420;
    float x424;
    float x428;
    float x42C;
    float x430;
    float x434;
    float x438;
    float x43C;
    float x440;
    float x444;
    float x448;
    float x44C;
    float x450;
    float x454;
    float x458;
    float x45C;
    float x460;
    float x464;
    float x468;
    float x46C;
    float x470;
    float x474;
    float x478;
    float x47C;
    float x480;
    float x484;
    float x488;
    float x48C;
    float x490;
    float x494;
    float x498;
    float x49C;
    float x4A0;
    float x4A4;
    float x4A8;
    float x4AC;
    float x4B0;
    float x4B4;
    float x4B8;
    float x4BC;
    float x4C0;
    float x4C4;
    float x4C8;
    float x4CC;
    float x4D0;
    float x4D4;
    float x4D8;
    float x4DC;
    float x4E0;
    float x4E4;
    float x4E8;
    float x4EC;
    float x4F0;
    float x4F4;
    float x4F8;
    float x4FC;
    float x500;
    float x504;
    float x508;
    float x50C;
    float x510;
    float x514;
    float x518;
    float x51C;
    float x520;
    float x524;
    float x528;
    float x52C;
    void dumpfunc() {
      dumpval(x000_Text, x400, x404, x408, x40C, x410);
      dumpval(x414, x418, x41C, x420, x424, x428);
      dumpval(x42C, x430, x434, x438, x43C, x440);
      dumpval(x444, x448, x44C, x450, x454, x458);
      dumpval(x45C, x460, x464, x468, x46C, x470);
      dumpval(x474, x478, x47C, x480, x484, x488);
      dumpval(x48C, x490, x494, x498, x49C, x4A0);
      dumpval(x4A4, x4A8, x4AC, x4B0, x4B4, x4B8);
      dumpval(x4BC, x4C0, x4C4, x4C8, x4CC, x4D0);
      dumpval(x4D4, x4D8, x4DC, x4E0, x4E4, x4E8);
      dumpval(x4EC, x4F0, x4F4, x4F8, x4FC, x500);
      dumpval(x504, x508, x50C, x510, x514, x518);
      dumpval(x51C, x520, x524, x528, x52C);
    }
  };

  declstruct(RecipeEntry) {
    char x000_Text[256];
    int x100;
    int x104;
    Sno<Recipe> x108_RecipeSno;
    int x10C_Enum; // Blacksmith = 0, Jeweler = 1, Mystic = 2, Jewel Upgrade = 3, Horadrim = 4, - None - = -1
    int x110;
    int x114;
    int x118;
    int x11C;
    RecipeIngredient x120_RecipeIngredients[6];
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108_RecipeSno, x10C_Enum, x110);
      dumpval(x114, x118, x11C, x120_RecipeIngredients);
    }
  };

  declstruct(ScriptedAchievementEvent) {
    char x000_Text[256];
    int x100;
    int x104;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104);
    }
  };

  declstruct(QuestSpecifier) {
    Sno<Quest> x00_QuestSno;
    float x04;
    GameBalanceId x08_GameBalanceId;
    GameBalanceId x0C_ItemsGameBalanceId;
    void dumpfunc() {
      dumpval(x00_QuestSno, x04, x08_GameBalanceId, x0C_ItemsGameBalanceId);
    }
  };

  declstruct(LootRunQuestTierEntry) {
    char x000_Text[256];
    int x100;
    int x104;
    QuestSpecifier x108_Quests[16];
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108_Quests);
    }
  };

  declstruct(ParagonBonus) {
    char x000_Text[256];
    int x100;
    int x104;
    int x108;
    uint32 x10C_;
    AttributeSpecifier x110_AttributeSpecifiers[4];
    int x170;
    int x174;
    int x178_Enum; // - None - = -1, DemonHunter = 0, Barbarian = 1, Wizard = 2, Witchdoctor = 3, Monk = 4, Crusader = 5
    char x17C_Text[256];
    uint32 x27C_;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108, x110_AttributeSpecifiers, x170);
      dumpval(x174, x178_Enum, x17C_Text);
    }
  };

  declstruct(LegacyItemConversion) {
    char x000_Text[256];
    int x100;
    int x104;
    GameBalanceId x108_ItemsGameBalanceId;
    GameBalanceId x10C_ItemsGameBalanceId;
    int x110;
    int x114;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108_ItemsGameBalanceId, x10C_ItemsGameBalanceId, x110);
      dumpval(x114);
    }
  };

  declstruct(EnchantItemAffixUseCountCostScalar) {
    int x00;
    float x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  declstruct(TieredLootRunLevel) {
    float x00;
    float x04;
    float x08;
    float x0C;
    float x10;
    float x14;
    float x18;
    float x1C;
    int x20;
    int x24;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
      dumpval(x18, x1C, x20, x24);
    }
  };

  declstruct(TransmuteRecipeIngredient) {
    int x00;
    GameBalanceId x04_GameBalanceId;
    int x08;
    void dumpfunc() {
      dumpval(x00, x04_GameBalanceId, x08);
    }
  };

  declstruct(TransmuteRecipe) {
    char x000_Text[256];
    int x100;
    int x104;
    int x108_Enum; // - None - = -1, [EXTRACT LEGENDARY POWER] = 0, [REFORGE LEGENDARY] = 1, [UPGRADE RARE ITEM] = 2
    TransmuteRecipeIngredient x10C_TransmuteRecipeIngredients[8];
    int x16C;
    int x170_;
    int x174_;
    void dumpfunc() {
      dumpval(x000_Text, x100, x104, x108_Enum, x10C_TransmuteRecipeIngredients, x16C);
    }
  };

  template<class T>
  class GameBalanceTable {
  private:
    SerializeData x00_SerializeData;
    Array<T> x08_Entries = x00_SerializeData;
  public:
    void serialize(json::Visitor* visitor) {
      x08_Entries.serialize(visitor);
    };
    T const& operator[](uint32 index) const {
      return x08_Entries[index];
    }
    T& operator[](uint32 index) {
      return x08_Entries[index];
    }

    T const* begin() const {
      return x08_Entries.begin();
    }
    T const* end() const {
      return x08_Entries.end();
    }
    T* begin() {
      return x08_Entries.begin();
    }
    T* end() {
      return x08_Entries.end();
    }
  
    T const* data() const {
      return x08_Entries.data();
    }
    uint32 size() const {
      return x08_Entries.size();
    }
  };

  SnoHeader x000_Header;
  int x00C;
  int x010;
  int x014;
  GameBalanceTable<ItemType> x018_ItemTypes;
  GameBalanceTable<Item> x028_Items;
  GameBalanceTable<ExperienceLevel> x038_ExperienceTable;
  GameBalanceTable<ExperienceLevelAlt> x048_ExperienceTableAlt;
  HelpCodes x058_HelpCodes;
  GameBalanceTable<MonsterLevelDefinition> x068_MonsterLevelTable;
  GameBalanceTable<AffixTableEntry> x078_AffixTable;
  GameBalanceTable<HeroData> x088_Heros;
  GameBalanceTable<MovementStyleDefinition> x098_MovementStyles;
  GameBalanceTable<LabelGBID> x0A8_Labels;
  GameBalanceTable<LootDistribution> x0B8_LootDistributionTable;
  GameBalanceTable<RareItemNamesEntry> x0C8_RareItemNamesTable;
  GameBalanceTable<MonsterAffixesEntry> x0D8_MonsterAffixesTable;
  GameBalanceTable<RareMonsterNamesEntry> x0E8_RareMonsterNamesTable;
  GameBalanceTable<SocketedEffectsTableEntry> x0F8_SocketedEffectsTable;
  GameBalanceTable<ItemDropTableEntry> x108_ItemDropTable;
  GameBalanceTable<LootDistribution> x118_ItemLevelModTable;
  GameBalanceTable<QualityClass> x128_QualityClassTable;
  GameBalanceTable<HandicapLevel> x138_HandicapLevelTable;
  GameBalanceTable<ItemSalvageLevel> x148_ItemSalvageLevelTable;
  GameBalanceTable<HirelingEntry> x158_Hirelings;
  GameBalanceTable<SetItemBonusTableEntry> x168_SetItemBonusTable;
  GameBalanceTable<EliteModifierData> x178_EliteModifiers;
  GameBalanceTable<ItemTierData> x188_ItemTiers;
  GameBalanceTable<PowerFormulaTableEntry> x198_PowerFormulaTable;
  GameBalanceTable<RecipeEntry> x1A8_RecipesTable;
  GameBalanceTable<ScriptedAchievementEvent> x1B8_ScriptedAchievementEventsTable;
  GameBalanceTable<LootRunQuestTierEntry> x1C8_LootRunQuestTierTable;
  GameBalanceTable<ParagonBonus> x1D8_ParagonBonusesTable;
  GameBalanceTable<LegacyItemConversion> x1E8_LegacyItemConversionTable;
  GameBalanceTable<EnchantItemAffixUseCountCostScalar> x1F8_EnchantItemAffixUseCountCostScalarsTable;
  GameBalanceTable<TieredLootRunLevel> x208_TieredLootRunLevelTable;
  GameBalanceTable<TransmuteRecipe> x218_TransmuteRecipesTable;
  void dumpfunc() {
    dumpval(x000_Header, x00C, x010, x014, x018_ItemTypes, x028_Items);
    dumpval(x038_ExperienceTable, x048_ExperienceTableAlt, x058_HelpCodes, x068_MonsterLevelTable, x078_AffixTable, x088_Heros);
    dumpval(x098_MovementStyles, x0A8_Labels, x0B8_LootDistributionTable, x0C8_RareItemNamesTable, x0D8_MonsterAffixesTable, x0E8_RareMonsterNamesTable);
    dumpval(x0F8_SocketedEffectsTable, x108_ItemDropTable, x118_ItemLevelModTable, x128_QualityClassTable, x138_HandicapLevelTable, x148_ItemSalvageLevelTable);
    dumpval(x158_Hirelings, x168_SetItemBonusTable, x178_EliteModifiers, x188_ItemTiers, x198_PowerFormulaTable, x1A8_RecipesTable);
    dumpval(x1B8_ScriptedAchievementEventsTable, x1C8_LootRunQuestTierTable, x1D8_ParagonBonusesTable, x1E8_LegacyItemConversionTable, x1F8_EnchantItemAffixUseCountCostScalarsTable, x208_TieredLootRunLevelTable);
    dumpval(x218_TransmuteRecipesTable);
  }
};

#pragma pack(pop)
