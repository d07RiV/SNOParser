#include "translations.h"
#include <cctype>
#include <set>
#include "parser.h"
#include "textures.h"
#include "types/SkillKit.h"
#include "types/Actor.h"
#include "types/AnimSet.h"
#include "types/Anim.h"
#include "types/GameBalance.h"
#include "powertag.h"
#include "strings.h"
#include "affixes.h"

std::string canonize(std::string const& str) {
  std::string dst;
  for (char chr : str) {
    if (std::isalpha(chr)) {
      dst.push_back(std::tolower(chr));
    }
  }
  return dst;
}

void SkillTips::read(std::string const& cls) {
  charClass = cls;
  File file("skilltip/skilltip_" + cls + ".js");
  std::string line;
  file.getline(line);
  file.getline(line);
  while (file.getc() != '{') {
    // no-op
  }
  file.seek(-1, SEEK_CUR);
  json::parse(file, skills, json::mJS);
  while (file.getc() != '{') {
    // no-op
  }
  file.seek(-1, SEEK_CUR);
  json::parse(file, passives, json::mJS);
}
void SkillTips::write() {
  File file("skilltip/skilltip_" + charClass + ".js", "w");
  file.printf("if (!DiabloCalc.skilltips) DiabloCalc.skilltips = {};\n");
  file.printf("if (!DiabloCalc.passivetips) DiabloCalc.passivetips = {};\n");
  file.printf("DiabloCalc.skilltips.%s = ", charClass.c_str());
  {
    json::WriterVisitor writer(file, json::mJS);
    writer.setIndent(2);
    skills.walk(&writer);
  }
  file.printf(";\nDiabloCalc.passivetips.%s = ", charClass.c_str());
  {
    json::WriterVisitor writer(file, json::mJS);
    writer.setIndent(2);
    passives.walk(&writer);
  }
  file.printf(";\n");
}
std::string SkillTips::format(std::string const& power) {
  PowerTag* tag = PowerTags::get(power);
  std::string tip = fmtstring("<div class=\"tooltip-body \"> <span class=\"d3-icon d3-icon-skill d3-icon-skill-64 \""
    " style=\"background-image: url('http://media.blizzard.com/d3/icons/skills/64/%s.png'); width: 64px; height: "
    "64px;\"> <span class=\"frame\"></span> </span> <div class=\"description\"> <p>", strlower(power).c_str());

  auto dictPowers = Strings::list("Powers");
  AttributeMap attr = GameAffixes::defaultMap();
  std::string stats = dictPowers[power + "_desc"];
  tip.append(FormatDescription(stats, true, attr, tag));

  //Image icon = GameTextures::get(tag->getint("Icon Normal"));
  //if (icon) {
  //  icon.write("skilltip/images/" + power + ".png");
  //}

  uint32 required = tag->getint("Item Type Requirement");
  char const* reqType = SnoManager::gameBalance()[required];
  if (reqType) {
    tip.append(fmtstring("</p> <p>Requires <span class=\"value\">%s</span>", reqType));
  }
  tip.append("</p> </div> </div>");
  return tip;
}
void SkillTips::generate(std::string const& cls, uint32 kitId) {
  charClass = cls;

  std::map<uint32, std::string> catIds;

  SnoFile<SkillKit> kit(SkillKit::name(kitId));
  for (auto& skill : kit->x20_ActiveSkillEntries) {
    catIds[skill.x08];
  }
  static struct { char const* cls; char const* cat; } catMap[] = {
    { "wizard", "Wiz" },
    { "monk", "Monk" },
    { "demonhunter", "DHunter" },
    { "witchdoctor", "WitchDoc" },
    { "barbarian", "Barb" },
    { "crusader", "Crusader" },
  };
  char const* catId = "";
  for (auto& cm : catMap) {
    if (cm.cls == cls) {
      catId = cm.cat;
      break;
    }
  }
  int index = 0;
  auto dictSkillsUI = Strings::list("SkillsUI");
  for (auto& kv : catIds) {
    kv.second = dictSkillsUI.getfmt("Cat_%s%d", catId, ++index);
  }

  static char const* elements[] = {
    "phy", "fir", "lit", "col", "psn", "arc", "hol"
  };

  auto dictPowers = Strings::list("Powers");
  auto dictAttributes = Strings::list("AttributeDescriptions");

  AttributeMap attr = GameAffixes::defaultMap();
  for (auto& skill : kit->x20_ActiveSkillEntries) {
    std::string power = skill.x00_PowerSno.name();
    PowerTag* tag = PowerTags::get(power);
    std::string tip = fmtstring("<div class=\"tooltip-body \"> <span class=\"d3-icon d3-icon-skill d3-icon-skill-64 \""
      " style=\"background-image: url('http://media.blizzard.com/d3/icons/skills/64/%s.png'); width: 64px; height: "
      "64px;\"> <span class=\"frame\"></span> </span> <div class=\"description\"> <p>", strlower(power).c_str());

    std::string name = dictPowers[power + "_name"];
    auto& val = skills[canonize(name)];
    auto& elems = val["elements"];
    std::string stats = dictPowers[power + "_desc"];
    tip.append(FormatDescription(stats, true, attr, tag));

    if (name == "Blessed Shield") {
      int asdf = 0;
    }

    Image icon = GameTextures::get(tag->getint("Icon Normal"));
    if (icon) {
      icon.write("skilltip/images/" + power + ".png");
    }

    uint32 required = tag->getint("Item Type Requirement");
    char const* reqType = SnoManager::gameBalance()[required];
    if (reqType) {
      tip.append(fmtstring("</p> <p>Requires <span class=\"value\">%s</span>", reqType));
    }
    tip.append(fmtstring("</p> <p class=\"special\">%s</p> <p class=\"subtle\">Unlocked at level <em>%d</em></p> </div> </div>",
      catIds[skill.x08].c_str(), skill.x0C));

    val["x"] = tip;
    elems["x"] = elements[tag->getint("NoRune Damage Type")];
    for (char rune = 'a'; rune <= 'e'; ++rune) {
      AttributeMap rattr = attr;
      rattr.emplace(fmtstring("Rune_%c", rune - 'a' + 'A'), 1.0);

      std::string desc = dictAttributes.getfmt("Rune_%c#%s", rune - 'a' + 'A', power.c_str());
      std::string subtip = "<p>";
      subtip.append(FormatDescription(desc, true, rattr, tag));
      subtip.append(fmtstring("</p> <p class=\"subtle\">Unlocked at level <em>%d</em></p>", (&skill.x14)[rune - 'a']));
      val[std::string{ rune }] = subtip;
      elems[std::string{ rune }] = elements[static_cast<int>(tag->get(fmtstring("Rune%c Damage Type", rune - 'a' + 'A'), rattr).max)];
    }
  }
  attr.emplace("sLevel", 1);
  for (auto& trait : kit->x10_TraitEntries) {
    if (trait.x04) continue;
    std::string power = trait.x00_PowerSno.name();
    PowerTag* tag = PowerTags::get(power);
    std::string tip = fmtstring("<div class=\"tooltip-body \"> <span class=\"d3-icon d3-icon-trait d3-icon-trait-64 \""
      " style=\"background-image: url('http://media.blizzard.com/d3/icons/skills/64/%s.png'); width: 64px; height: "
      "64px;\"> <span class=\"frame\"></span> </span> <div class=\"description\"> <p>", strlower(power).c_str());

    Image icon = GameTextures::get(tag->getint("Icon Normal"));
    Image icondis = GameTextures::get(tag->getint("Icon Inactive"));
    if (icon) {
      icon.write("skilltip/images/" + power + ".png");
    }
    if (icondis) {
      icondis.write("skilltip/inactive/" + power + ".png");
    }

    std::string name = dictPowers[power + "_name"];
    std::string stats = dictPowers[power + "_var_stats"];
    tip.append(FormatDescription(stats, true, attr, tag));

    tip.append(fmtstring("</p> <p class=\"subtle\">Unlocked at level <em>%d</em></p> </div> </div> "
      "<div class=\"tooltip-extension \"> <div class=\"flavor\">", trait.x08));
    tip.append(dictPowers[power + "_desc"]);
    tip.append("</div> </div>");
    passives[canonize(name)] = tip;
  }
}
void SkillTips::dump() {
  SnoFile<GameBalance> gmb("Characters");
  void* task = Logger::begin(gmb->x088_Heros.size(), "Parsing skills");
  for (auto& chr : gmb->x088_Heros) {
    SkillTips data;
    Logger::item(chr.x000_Text, task);
    data.generate(canonize(chr.x000_Text), chr.x120_SkillKitSno);
    data.write();
  }
  Logger::end(false, task);
}

struct RuneEntry {
  uint32 level;
  uint32 index;
};
struct TraitEntry {
  uint32 level;
  uint32 power;
};
bool operator < (RuneEntry const& lhs, RuneEntry const& rhs) {
  return lhs.level < rhs.level;
}
bool operator < (TraitEntry const& lhs, TraitEntry const& rhs) {
  return lhs.level < rhs.level;
}
struct SkillEntry {
  uint32 level;
  uint32 category;
  uint32 power;
  int runes[5];
};
bool operator < (SkillEntry const& lhs, SkillEntry const& rhs) {
  if (lhs.category != rhs.category) return lhs.category < rhs.category;
  return lhs.level < rhs.level;
}

void simBase(std::string const& name) {
  File out(strlower(name) + ".js", "w");
  SnoFile<GameBalance> heroes("Characters");
  std::string kitName;
  std::string actorName;
  for (auto& hero : heroes->x088_Heros) {
    if (strlower(hero.x000_Text) != strlower(name)) continue;
    kitName = hero.x120_SkillKitSno.name();
    actorName = hero.x108_ActorSno.name();
    break;
  }
  if (kitName.empty()) return;

  SnoFile<Actor> actor(actorName);
  SnoFile<AnimSet> animSet(actor->x068_AnimSetSno.name());
  std::map<uint32, uint32> animMap;
  std::map<uint32, double> animLength;
  uint32 prio[] = {
    2, // 1H Swing,
    3, // 1H Thrust,
    4, // 2H Swing,
    5, // 2H Thrust,
    7, // Bow,
    8, // Crossbow,
    9, // Wand,
    26, // 2H Flail,
    16, // 1H Fist,
    17, // 2H Axe / Mace,
    18, // HandXBow,
    19, // Wand w / Orb,
    20, // 1H Swing w / Shield,
    21, // 1H Thrust w / Shield,
    22, // Hth w / Shield,
    23, // 2H Swing w / Shield,
    24, // 2H Thrust w / Shield,
    25, // Staff w / Shield,
    27, // 2H Flail w / Shield,
    6, // Staff,
    0, // None,
    1, // Hand - to - hand,
    10, // Dual Wield,
    11, // HtH w / Orb,
    12, // 1HS w / Orb,
    13, // 1HT w / Orb,
    14, // Dual Wield SF,
    15, // Dual Wield FF,
    28, // On Horse,
  };
  std::reverse(prio, prio + 29);
  for (uint32 id : prio) {
    auto& tag = (id == 0 ? animSet->x018_TagMap : animSet->x020_AnimSetTagMaps[id - 1].x08_TagMap);
    if (tag.size() == 0 || tag.size() != tag[0] * 3 + 1) continue;
    for (uint32 i = 0; i < tag[0]; ++i) {
      if (tag[i * 3 + 1] != 2) break;
      animMap[tag[i * 3 + 2]] = tag[i * 3 + 3];
    }
  }

  SnoFile<SkillKit> kit(kitName);
  std::vector<SkillEntry> skills;
  std::vector<TraitEntry> traits;
  for (auto& skill : kit->x20_ActiveSkillEntries) {
    skills.emplace_back();
    auto& entry = skills.back();
    entry.category = skill.x08;
    entry.level = skill.x0C;
    entry.power = skill.x00_PowerSno;
    RuneEntry runes[5];
    for (int i = 0; i < 5; ++i) {
      runes[i].index = i;
      runes[i].level = (&skill.x14)[i];
    }
    std::sort(runes, runes + 5);
    for (int i = 0; i < 5; ++i) {
      entry.runes[i] = runes[i].index;
    }
  }
  for (auto& trait : kit->x10_TraitEntries) {
    traits.emplace_back();
    auto& entry = traits.back();
    entry.level = trait.x08;
    entry.power = trait.x00_PowerSno;
  }
  std::sort(skills.begin(), skills.end());
  std::sort(traits.begin(), traits.end());

  static char const* elements[] = {
    "phy", "fir", "lit", "col", "psn", "arc", "hol"
  };

  auto dictPowers = Strings::list("Powers");
  auto dictAttributes = Strings::list("AttributeDescriptions");
  AttributeMap attr = GameAffixes::defaultMap();

  for (auto& entry : skills) {
    std::string power = Power::name(entry.power);
    PowerTag* tag = PowerTags::get(power);
    std::string name = dictPowers[power + "_name"];

    out.printf("  skills.%s = {\n", canonize(name).c_str());
    if (entry.category == skills[0].category) {
      out.printf("    signature: true,\n");
    }
    if (tag->getint("IsOffensive")) {
      out.printf("    offensive: true,\n");
    }
    if (!tag->getint("IsPrimary")) {
      out.printf("    secondary: true,\n");
    }
    std::vector<double> gen(6);
    std::vector<double> proc(6);
    std::vector<int> elem(6);
    gen[0] = tag->get("Resource Gained On First Hit", attr).max;
    proc[0] = tag->get("NoRune Proc Scalar", attr).max;
    elem[0] = tag->getint("NoRune Damage Type");
    for (int i = 0; i < 5; ++i) {
      AttributeMap rattr = attr;
      rattr[fmtstring("Rune_%c", 'A' + i)] = 1;
      gen[i + 1] = tag->get("Resource Gained On First Hit", rattr).max;
      proc[i + 1] = tag->get(fmtstring("Rune%c Proc Scalar", 'A' + i), rattr).max;
      elem[i + 1] = tag->getint(fmtstring("Rune%c Damage Type", 'A' + i));
    }

    if (std::min_element(gen.begin(), gen.end()) != std::max_element(gen.begin(), gen.end())) {
      out.printf("    generate: {");
      bool first = true;
      if (gen[0]) {
        out.printf("x: %g", gen[0]);
        first = false;
      }
      for (int rune : entry.runes) {
        if (gen[rune + 1]) {
          if (!first) out.printf(", ");
          out.printf("%c: %g", 'a' + rune, gen[rune + 1]);
          first = false;
        }
      }
      out.printf("},\n");
    } else if (gen[0]) {
      out.printf("    generate: %g,\n", gen[0]);
    }

    uint32 animTag = tag->getint("Animation Tag");
    if (animMap.count(animTag)) {
      uint32 animFile = animMap[animTag];
      if (!animLength.count(animFile)) {
        SnoFile<Anim> anim(Anim::name(animFile));
        if (anim && anim->x28_AnimPermutations.size()) {
          auto& ap = anim->x28_AnimPermutations[0];
          animLength[animFile] = (ap.x090 - 1) / ap.x048_Velocity;
        }
      }
      out.printf("    speed: %.8g,\n", animLength[animFile]);
    }

    out.printf("    oncast: function(rune) {\n");
    out.printf("    },\n");

    if (std::min_element(proc.begin(), proc.end()) != std::max_element(proc.begin(), proc.end())) {
      out.printf("    proctable: {");
      bool first = true;
      if (proc[0]) {
        out.printf("x: %g", proc[0]);
        first = false;
      }
      for (int rune : entry.runes) {
        if (proc[rune + 1]) {
          if (!first) out.printf(", ");
          out.printf("%c: %g", 'a' + rune, proc[rune + 1]);
          first = false;
        }
      }
      out.printf("},\n");
    } else if (proc[0]) {
      out.printf("    proctable: %g,\n", proc[0]);
    }

    if (std::min_element(elem.begin(), elem.end()) != std::max_element(elem.begin(), elem.end())) {
      out.printf("    elem: {x: \"%s\"", elements[elem[0]]);
      for (int rune : entry.runes) {
        out.printf(", %c: \"%s\"", 'a' + rune, elements[elem[rune + 1]]);
      }
      out.printf("},\n");
    } else {
      out.printf("    elem: \"%s\",\n", elements[elem[0]]);
    }

    out.printf("  };\n\n");
  }

  out.printf("  Sim.passives = {\n");
  for (auto& entry : traits) {
    std::string power = Power::name(entry.power);
    std::string name = dictPowers[power + "_name"];
    out.printf("    %s: function() {\n", canonize(name).c_str());
    out.printf("    },\n");
  }
  out.printf("  };\n");
}
