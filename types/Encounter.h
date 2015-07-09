#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Encounter::Type) {
  declstruct(EncounterSpawnOption) {
    Sno<Actor> x00_ActorSno;
    int x04;
    int x08;
    Sno<Condition> x0C_ConditionSno;
    void dumpfunc() {
      dumpval(x00_ActorSno, x04, x08, x0C_ConditionSno);
    }
  };

  SnoHeader x00_Header;
  Sno<Actor> x0C_ActorSno;
  Array<EncounterSpawnOption> x10_EncounterSpawnOptions = x18_SerializeData;
  SerializeData x18_SerializeData;
  void dumpfunc() {
    dumpval(x00_Header, x0C_ActorSno, x10_EncounterSpawnOptions);
  }
};

#pragma pack(pop)
