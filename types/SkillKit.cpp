#include "SkillKit.h"
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

void dumpKits(std::map<std::string, std::string>& strings) {
  File output(path::work() / "kits.js", "wt");
  output.printf("var Passives = {};\nvar Skills = {};\n");
  for (auto& kit : SnoLoader::default->all<SkillKit>()) {
    std::string title = kit.name();
    std::transform(title.begin(), title.end(), title.begin(), ::tolower);
    output.printf("Passives[\"%s\"] = {\n", title.c_str());
    for (auto& trait : kit->x10_TraitEntries) {
      std::string power = trait.x00_PowerSno.name();
      std::string powerName = strings[power + "_name"];
      output.printf("  \"%s\": \"%s\",\n", powerName.c_str(), power.c_str());
    }
    output.printf("};\n");
    output.printf("Skills[\"%s\"] = {\n", title.c_str());
    for (auto& skill : kit->x20_ActiveSkillEntries) {
      std::string power = skill.x00_PowerSno.name();
      std::string powerName = strings[power + "_name"];
      output.printf("  \"%s\": \"%s\",\n", powerName.c_str(), power.c_str());
    }
    output.printf("};\n");
  }
}
