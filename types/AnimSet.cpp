#include "snotypes.h"
#include "snomap.h"
#include <map>
#include <vector>
#include <string>
using namespace std;

static const char* AnimClasses[] = {
  "None",
  "Hand - to - hand",
  "1H Swing",
  "1H Thrust",
  "2H Swing",
  "2H Thrust",
  "Staff",
  "Bow",
  "Crossbow",
  "Wand",
  "Dual Wield",
  "HtH w / Orb",
  "1HS w / Orb",
  "1HT w / Orb",
  "Dual Wield SF",
  "Dual Wield FF",
  "1H Fist",
  "2H Axe / Mace",
  "HandXBow",
  "Wand w / Orb",
  "1H Swing w / Shield",
  "1H Thrust w / Shield",
  "Hth w / Shield",
  "2H Swing w / Shield",
  "2H Thrust w / Shield",
  "Staff w / Shield",
  "2H Flail",
  "2H Flail w / Shield",
  "On Horse",
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
  map<uint32, map<uint32, uint32>> anims;
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
      anims[src][i] = dst;
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
