#include <stdio.h>
#include <iostream>
#include <clocale>
#include <conio.h>
#include <set>
#include "snomap.h"
#include "strings.h"
#include "stringmgr.h"
#include "parser.h"
#include "json.h"
#include "path.h"
#include "image.h"
#include "textures.h"
#include "types/Textures.h"
#include "types/Appearance.h"
#include "types/Actor.h"
#include "types/Monster.h"
#include "types/AnimSet.h"
#include "types/StringList.h"
#include "types/SkillKit.h"
#include "regexp.h"
#include "translations.h"
#include "description.h"
#include "miner.h"
#include "powertag.h"
#include "model.h"
#include "itemlib.h"
#include "webgl.h"
#include "frameui/searchlist.h"
#include "affixes.h"
#include "server.h"
#include <map>
#include <vector>
#include <algorithm>
#include <stdarg.h>

namespace path {
  std::vector<std::string> roots;
  std::vector<std::string> cascs;
}

#include <shlobj.h>
bool browseForFolder(wchar_t const* prompt, std::string& result) {
  IFileDialog* pfd;
  if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd)))) {
    return false;
  }
  DWORD dwOptions;
  if (SUCCEEDED(pfd->GetOptions(&dwOptions))) {
    pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
  }
  pfd->SetTitle(prompt);
  if (FAILED(pfd->Show(NULL))) {
    pfd->Release();
    return false;
  }
  IShellItem* psi;
  if (FAILED(pfd->GetResult(&psi))) {
    pfd->Release();
    return false;
  }
  wchar_t* str;
  if (FAILED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &str)) || !str) {
    psi->Release();
    pfd->Release();
    return false;
  }
  std::wstring ws(str);
  result = std::string(ws.begin(), ws.end());
  psi->Release();
  pfd->Release();
  return true;
}

void make_menu(File& mf, json::Value lhs, json::Value rhs) {
  struct {
    std::string fn, name;
  } classes[] = {
      {"Barbarian", "Barbarian"},
      {"Demonhunter", "Demon Hunter"},
      {"Monk", "Monk"},
      {"WitchDoctor", "Witch Doctor"},
      {"Wizard", "Wizard"},
      {"X1_Crusader", "Crusader"},
      {"Templar", "Templar"},
      {"Scoundrel", "Scoundrel"},
      {"Enchantress", "Enchantress"},
  };

  jsonCompare(rhs, lhs);
  std::set<istring> plist;
  for (auto& kv : rhs.getMap()) plist.insert(kv.first);
  for (auto& kv : lhs.getMap()) plist.insert(kv.first);
  Dictionary pown = readStrings("Powers");
  mf.printf("  <div class=\"menu\">\n");
  for (auto& cls : classes) {
    SnoFile<SkillKit> kit(cls.fn);
    std::vector<std::string> skills, traits;
    for (auto& skill : kit->x20_ActiveSkillEntries) {
      std::string power = skill.x00_PowerSno.name();
      if (plist.find(power) != plist.end()) {
        skills.push_back(fmtstring("   <a href=\"#%s\">%s</a>\n", power.c_str(), pown[power + "_name"].c_str()));
      }
    }
    for (auto& trait : kit->x10_TraitEntries) {
      std::string power = trait.x00_PowerSno.name();
      if (plist.find(power) != plist.end()) {
        traits.push_back(fmtstring("   <a href=\"#%s\">%s</a>\n", power.c_str(), pown[power + "_name"].c_str()));
      }
    }
    if (!skills.empty() || !traits.empty()) {
      mf.printf("   <h4>%s</h4>\n", cls.name.c_str());
      for (auto& str : skills) mf.printf("%s", str);
      if (!skills.empty() && !traits.empty()) mf.printf("   <br/>\n");
      for (auto& str : traits) mf.printf("%s", str);
    }
  }
  mf.printf("  </div>");
}

void make_diff(char const* name, uint32 lhs, uint32 rhs, std::vector<std::string> const& fields, bool links = false) {
  json::Value vlhs, vrhs;
  json::parse(File(path::work() / fmtstring("%s.%d.js", name, lhs)), vlhs);
  json::parse(File(path::work() / fmtstring("%s.%d.js", name, rhs)), vrhs);

  File out(path::root() / fmtstring("diff/%s.%d.%d.html", name, lhs, rhs), "wb");
  out.copy(File(path::work() / fmtstring("templates/diff.%s.html", name)));
  if (links) make_menu(out, vlhs, vrhs);
  diff(out, vlhs, vrhs, fields, links);
  out.copy(File(path::work() / "templates/diff.tail.html"));
}

void make_html(char const* name, uint32 version, json::Value const& value, std::vector<std::string> const& fields, bool links = false) {
  File out(path::root() / "game" / fmtstring("%s.%d.html", name, version), "wb");
  out.copy(File(path::work() / fmtstring("templates/game.%s.html", name)));
  makehtml(out, value, fields, links);
  out.copy(File(path::work() / "templates/game.tail.html"));
}

void write_icon(Archive& dsticons, GameBalance::Type::Item const& item) {
  static uint32 chestType = HashNameLower("ChestArmor");
  uint32 type = item.x10C_ItemTypesGameBalanceId;
  while (type && type != -1 && type != chestType) {
    type = GameAffixes::itemTypeParent(type);
  }
  bool torso = (type == chestType);

  SnoFile<Actor> actor(item.x108_ActorSno.name());
  if (!actor) return;
  auto& images = actor->x310_InventoryImages;
  std::set<uint32> icons;
  for (auto& inv : images) {
    if (inv.x00 != 0 && inv.x00 != -1) icons.insert(inv.x00);
    if (inv.x04 != 0 && inv.x04 != -1) icons.insert(inv.x04);
  }
  for (uint32 id : icons) {
    if (dsticons.has(id)) continue;
    Image image = GameTextures::get(id);
    if (image) {
      if (torso && (image.width() != 82 || image.height() != 164)) {
        Image tmp(82, 164);
        tmp.blt(41 - image.width() / 2, 82 - image.height() / 2, image);
        image = tmp;
      }
      image.write(dsticons.create(id), ImageFormat::PNG);
    }
  }
}

static json::Value versions;

void OpExtract() {
  uint32 build = SnoLoader::default->build();
  std::string suffix = fmtstring(".%d.js", build);
  json::Value js_sets;
  json::Value js_items;
  json::Value js_powers;
  for (auto& gmb : SnoLoader::All<GameBalance>()) {
    for (auto& val : gmb->x168_SetItemBonusTable) {
      parseSetBonus(val, js_sets, false);
    }
    for (auto& val : gmb->x028_Items) {
      parseItem(val, js_items, false);
    }
  }
  for (auto& pow : SnoLoader::All<Power>()) {
    parsePower(*pow, js_powers, false);
  }
  json::write(File(path::work() / "itemsets" + suffix, "w"), js_sets);
  json::write(File(path::work() / "items" + suffix, "w"), js_items);
  json::write(File(path::work() / "powers" + suffix, "w"), js_powers);

  make_html("items", build, js_items, { "name", "set", "icon", "flavor", "primary", "secondary", "powers" });
  make_html("itemsets", build, js_sets, { "name", "2", "3", "4", "5", "6", "powers" });
}

void OpCompare() {
  uint32 build = SnoLoader::default->build();
  if (!File::exists(path::work() / fmtstring("items.%d.js", build))) {
    Logger::log("Please extract game data first");
    return;
  }
  std::vector<uint32> verlist;
  for (auto const& kv : versions.getMap()) {
    uint32 ver = std::stoi(kv.first);
    if (ver != build && File::exists(path::work() / fmtstring("items.%d.js", ver))) {
      verlist.push_back(ver);
    }
  }
  if (verlist.empty()) {
    Logger::log("No other versions found");
    return;
  }
  std::sort(verlist.begin(), verlist.end(), [](uint32 a, uint32 b) { return a > b; });
  std::vector<std::string> vernames;
  for (uint32 v : verlist) {
    vernames.push_back(versions[fmtstring("%d", v)].getString());
  }
  int index = Logger::menu("Choose other version", vernames);
  uint32 other = verlist[index];

  make_diff("items", other, build, { "name", "set", "icon", "flavor", "primary", "secondary", "powers" });
  make_diff("itemsets", other, build, { "name", "2", "3", "4", "5", "6", "powers" });
  make_diff("powers", other, build, { "name", "desc", "flavor", "rune_a", "rune_b", "rune_c", "rune_d", "rune_e" }, true);
}

void OpDumpSNO() {
  std::vector<std::string> types;
  std::vector<uint32> indices;
#define SNOTYPE(T)  types.push_back(T::type()); indices.push_back(T::index);
#include "allsno.h"
#undef SNOTYPE
  types.push_back("Cancel");
  indices.push_back(0);
  int choice = Logger::menu("Choose SNO type", types);
  switch (indices[choice]) {
#define SNOTYPE(T)  case T::index: SnoLoader::Dump<T>(); break;
#include "allsno.h"
#undef SNOTYPE
  }
}

void OpExtractIcons() {
  Archive icons;
  for (auto& gmb : SnoLoader::All<GameBalance>()) {
    for (auto& item : gmb->x028_Items) {
      write_icon(icons, item);
    }
  }
  icons.write(File("icons.wgz", "wb"), false);
}

void OpDumpPowers() {
  json::Value value = PowerTags::dump();
  File out(path::work() / fmtstring("jspowers.%d.js", SnoLoader::default->build()), "wb");
  out.printf("var Powers=");
  json::WriterVisitor visitor(out, json::mJS);
  value.walk(&visitor);
  visitor.onEnd();
  out.printf(";");
}

//{"Extract game data", "Compare versions", "Dump SNO data", "Extract icons", "Model viewer", "Exit"}
struct {
  char const* name;
  void(*func)();
} operations[] = {
  { "Extract game data", OpExtract },
  { "Compare versions", OpCompare },
  { "Dump SNO data", OpDumpSNO },
  { "Extract icons", OpExtractIcons },
  { "Model viewer", ViewModels },
  { "Dump powers", OpDumpPowers },
  { "Exit", nullptr },
};

int do_main() {
  json::Value value;
  json::parse(File(path::work() / "paths.js"), value);
  while (true) {
    std::vector<std::string> game_options = {"Download from CDN", "Browse for folder"};
    if (value.type() == json::Value::tArray) {
      game_options.push_back("Previously loaded folders");
    }
    game_options.push_back("Browse extracted data");
    game_options.push_back("Exit");
    int game_choice = Logger::menu("Choose game", game_options);
    if (game_choice == 0) {
      auto builds = SnoCdnLoader::builds();
      std::vector<std::string> build_names;
      size_t pos = 0;
      int choice;
      do {
        std::map<char, std::string> options;
        for (size_t i = pos; i < pos + 9 && i < builds.size(); ++i) {
          if (i >= build_names.size()) {
            build_names.push_back(SnoCdnLoader::buildinfo(builds[i])["build-name"]);
          }
          options['1' + (i - pos)] = build_names[i];
        }
        if (pos + 9 < builds.size()) options['N'] = "<Next>";
        if (pos > 0) options['P'] = "<Prev>";
        choice = Logger::menu("Choose build", options);
        if (choice == 'N') {
          pos += 9;
        } else if (choice == 'P') {
          pos -= 9;
          if (pos < 0) pos = 0;
        }
      } while (choice < '1' || choice > '9');
      pos += (choice - '1');
      if (pos >= builds.size()) throw Exception("invalid build number");
      SnoLoader::default = new SnoCdnLoader(builds[pos], "choose");
      break;
    } else if (game_choice == game_options.size() - 1) {
      return 0;
    } else if (game_choice == game_options.size() - 2) {
      Server srv;
      srv.run();
      return 0;
    } else {
      std::string dir;
      if (game_choice == 1) {
        if (!browseForFolder(L"Locate D3 folder", dir)) {
          continue;
        }
      } else {
        std::vector<std::string> opts = {"Back"};
        for (auto const& p : value) {
          int attr = GetFileAttributes(p.getString().c_str());
          if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
            opts.push_back(p.getString());
          }
        }
        int choice = Logger::menu("Choose folder", opts);
        if (choice == 0) continue;
        dir = opts[choice - 1];
      }
      if (game_choice == 2) {
        value.append(dir);
        json::write(File(path::work() / "paths.js", "wb"), value);
      }
      if (GetFileAttributes((dir / ".build.info").c_str()) != INVALID_FILE_ATTRIBUTES) {
        dir = dir / "Data";
      }
      SnoLoader::default = new SnoCascLoader(dir, "choose");
      break;
    }
  }
  Logger::log("Loaded build %d", SnoLoader::default->build());
  json::parse(File(path::work() / "versions.js"), versions);
  versions[fmtstring("%d", SnoLoader::default->build())] = SnoLoader::default->version();
  json::write(File(path::work() / "versions.js", "wb"), versions);

  while (true) {
    std::vector<std::string> opNames;
    for (auto const& op : operations) {
      opNames.push_back(op.name);
    }
    int choice = Logger::menu("Choose action", opNames);
    if (operations[choice].func) {
      operations[choice].func();
    } else {
      break;
    }
  }
  return 0;
}

int main() {
  std::setlocale(LC_ALL, "eu_US.UTF-8");

  INITCOMMONCONTROLSEX iccex;
  iccex.dwSize = sizeof iccex;
  iccex.dwICC = ICC_STANDARD_CLASSES | ICC_PROGRESS_CLASS |
    ICC_BAR_CLASSES | ICC_TREEVIEW_CLASSES | ICC_LISTVIEW_CLASSES |
    ICC_TAB_CLASSES | ICC_UPDOWN_CLASS | ICC_DATE_CLASSES;
  InitCommonControlsEx(&iccex);
  OleInitialize(NULL);

  try {
    return do_main();
  } catch (Exception& ex) {
    fprintf(stderr, "Exception: %s\n", ex.what());
    fprintf(stderr, "Press any key to continue . . .\n");
    getch();
    return 1;
  }
}
