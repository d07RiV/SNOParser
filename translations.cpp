#include "translations.h"
#include <cctype>
#include <set>
#include "parser.h"
#include "types/SkillKit.h"
#include "types/GameBalance.h"
#include "powertag.h"
#include "strings.h"

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

  AttributeMap attr;
  attr.emplace("Attacks_Per_Second_Total", 1.0);
  attr.emplace("Level", 70.0);
  attr.emplace("Effective_Level", 70.0);
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

    uint32 required = static_cast<uint32>(tag->get("Item Type Requirement", attr).max);
    char const* reqType = SnoManager::gameBalance()[required];
    if (reqType) {
      tip.append(fmtstring("</p> <p>Requires <span class=\"value\">%s</span>", reqType));
    }
    tip.append(fmtstring("</p> <p class=\"special\">%s</p> <p class=\"subtle\">Unlocked at level <em>%d</em></p> </div> </div>",
      catIds[skill.x08].c_str(), skill.x0C));

    val["x"] = tip;
    elems["x"] = elements[static_cast<int>(tag->get("NoRune Damage Type").max)];
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
