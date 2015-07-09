#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Power::Type) {
  declstruct(ActorCollisionFlags) {
    int x00;
    int x04;
    int x08;
    int x0C;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C);
    }
  };

  declstruct(BuffFilterDef) {
    Sno<Power> x00_PowerSno;
    int x04;
    void dumpfunc() {
      dumpval(x00_PowerSno, x04);
    }
  };

  declstruct(BuffDef) {
    SerializeData x00_SerializeData;
    Array<BuffFilterDef> x08_BuffFilterDefs = x00_SerializeData;
    void dumpfunc() {
      dumpval(x08_BuffFilterDefs);
    }
  };

  struct PowerTags : public Array<uint32> {
    PowerTags(SerializeData const& sd)
      : Array<uint32>(sd)
    {}

    void serialize(json::Visitor* visitor);
  };

  declstruct(PowerDef) {
    SerializeData x000_SerializeData;
    PowerTags x008_TagMap = x000_SerializeData; // type: 0x00040000
    SerializeData x010_SerializeData;
    PowerTags x018_TagMap = x010_SerializeData; // type: 0x00130000
    SerializeData x020_SerializeData;
    PowerTags x028_TagMap = x020_SerializeData; // type: 0x00130000
    SerializeData x030_SerializeData;
    SerializeData x038_SerializeData;
    SerializeData x040_SerializeData;
    SerializeData x048_SerializeData;
    PowerTags x050_TagMap = x030_SerializeData; // type: 0x00020000
    PowerTags x058_TagMap = x038_SerializeData; // type: 0x00020000
    PowerTags x060_TagMap = x040_SerializeData; // type: 0x00020000
    PowerTags x068_TagMap = x048_SerializeData; // type: 0x00020000
    SerializeData x070_SerializeData;
    SerializeData x078_SerializeData;
    SerializeData x080_SerializeData;
    SerializeData x088_SerializeData;
    PowerTags x090_TagMap = x070_SerializeData; // type: 0x00020000
    PowerTags x098_TagMap = x078_SerializeData; // type: 0x00020000
    PowerTags x0A0_TagMap = x080_SerializeData; // type: 0x00020000
    PowerTags x0A8_TagMap = x088_SerializeData; // type: 0x00020000
    int x0B0;
    ActorCollisionFlags x0B4_ActorCollisionFlags;
    ActorCollisionFlags x0C4_ActorCollisionFlags;
    uint32 x0D4_;
    BuffDef x0D8_BuffDefs[32];
    void dumpfunc() {
      dumpval(x008_TagMap, x018_TagMap, x028_TagMap, x050_TagMap, x058_TagMap, x060_TagMap);
      dumpval(x068_TagMap, x090_TagMap, x098_TagMap, x0A0_TagMap, x0A8_TagMap, x0B0);
      dumpval(x0B4_ActorCollisionFlags, x0C4_ActorCollisionFlags, x0D8_BuffDefs);
    }
  };

  declstruct(ScriptFormulaDetails) {
    char x000_Text[256];
    char x100_Text[512];
    int x300;
    int x304;
    void dumpfunc() {
      dumpval(x000_Text, x100_Text, x300, x304);
    }
  };

  SnoHeader x000_Header;
  char x00C_Text[64];
  uint32 x04C_;
  PowerDef x050_PowerDef;
  int x328;
  int x32C;
  char x330_Text[256];
  SerializeData x430_SerializeData;
  Array<ScriptFormulaDetails> x438_ScriptFormulaDetails = x430_SerializeData;
  int x440;
  uint32 x444_;
  Array<uint8> x448_byte = x450_SerializeData;
  SerializeData x450_SerializeData;
  Sno<Quest> x458_QuestSno;
  uint32 x45C_;
  void dumpfunc() {
    dumpval(x000_Header, x00C_Text, x050_PowerDef, x328, x32C, x330_Text);
    dumpval(x438_ScriptFormulaDetails, x440, x448_byte, x458_QuestSno);
  }
};

#pragma pack(pop)
