#include "snotypes.h"
#include "snomap.h"
#include "snocommon.h"
#include <map>
#include <vector>
#include <string>
using namespace std;

static const char* AnimClasses[] = {
  /* 0*/ "None",
  /* 1*/ "Hand - to - hand",
  /* 2*/ "1H Swing",
  /* 3*/ "1H Thrust",
  /* 4*/ "2H Swing",
  /* 5*/ "2H Thrust",
  /* 6*/ "Staff",
  /* 7*/ "Bow",
  /* 8*/ "Crossbow",
  /* 9*/ "Wand",
  /*10*/ "Dual Wield",
  /*11*/ "HtH w / Orb",
  /*12*/ "1HS w / Orb",
  /*13*/ "1HT w / Orb",
  /*14*/ "Dual Wield SF",
  /*15*/ "Dual Wield FF",
  /*16*/ "1H Fist",
  /*17*/ "2H Axe / Mace",
  /*18*/ "HandXBow",
  /*19*/ "Wand w / Orb",
  /*20*/ "1H Swing w / Shield",
  /*21*/ "1H Thrust w / Shield",
  /*22*/ "Hth w / Shield",
  /*23*/ "2H Swing w / Shield",
  /*24*/ "2H Thrust w / Shield",
  /*25*/ "Staff w / Shield",
  /*26*/ "2H Flail",
  /*27*/ "2H Flail w / Shield",
  /*28*/ "On Horse",
};

map<uint32, vector<float>> const& BaseFPA();

void AnimSet::parse(File& in, json::Visitor* out) {
  auto _fpa = BaseFPA();
  const SnoMap& animMap = SnoManager::get<Anim>();
  in.seek(0);
  if (in.read32() != 0xDEADBEEF || in.read32() != 24) {
    return;
  }
  in.seek(16);
  uint32 SNO = in.read32();
  map<uint32, map<uint32, Sno<Anim>>> anims;
  for (int i = 0; i < 29; ++i) {
    in.seek(32 + i * 16);
    uint32 offs = in.read32();
    uint32 size = in.read32();
    in.seek(offs + 16);
    uint32 count = in.read32();
    if (size != 4 + count * 12) continue;
    while (count--) {
      if (in.read32() != 2) break;
      uint32 src = in.read32();
      uint32 dst = in.read32();
      anims[src][i].id = dst;
    }
  }

  out->onOpenMap();
  out->onMapKey("sno");
  out->onInteger(SNO);
  for (auto& x : anims) {
    out->onMapKey(fmtstring("%d", x.first));
    out->onOpenMap();
    for (auto& y : x.second) {
      //fprintf(*out, "  %s: %s\n", AnimClasses[y.first], y.second.c_str());
      char const* name = animMap[y.second];
      if (!name) continue;
      out->onMapKey(AnimClasses[y.first]);
      //y.second.serialize(out);
      out->onOpenMap();
      out->onMapKey("name");
      out->onString(name);
      out->onMapKey("fpa");
      auto& fpas = _fpa[y.second];
      if (fpas.size() == 1) {
        out->onNumber(fpas[0]);
      } else {
        out->onOpenArray();
        for (float fpa : fpas) {
          out->onNumber(fpa);
        }
        out->onCloseArray();
      }
      out->onCloseMap();
    }
    out->onCloseMap();
  }
  out->onCloseMap();
}
