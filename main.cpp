#include <stdio.h>
#include <iostream>
#include <clocale>
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
#include "regexp.h"
#include "translations.h"
#include "description.h"
#include "miner.h"
#include "powertag.h"
#include "model.h"
#include "itemlib.h"
#include "webgl.h"
#include <map>
#include <vector>
#include <algorithm>
#include <stdarg.h>
using namespace std;

namespace path {
  std::vector<std::string> roots {
    "C:\\Work\\junk",
    "C:\\Webroot\\game",
    "C:\\tmp",
  };
  std::vector<std::string> cascs {
#ifdef PTR
    "E:\\D3Ptr\\Data",
    "G:\\D3Ptr\\Data",
#else
    "E:\\D3Live\\Data",
    "G:\\D3Live\\Data",
#endif
  };
}

Model* ShowModel(Model* model) {
  for (uint32 i = 0; i < model->numSubObjects(); ++i) {
    //if (model->subObjectType(i) == 103) {
      model->appearance(i, 0);
    //}
  }
  return model;
}

std::string getItemAppearance(char const* id) {
  auto* item = ItemLibrary::get(id);
  if (!item) {
    SnoFile<Actor> actor(id);
    if (!actor) return std::string();
    return actor->x014_AppearanceSno.name().c_str();
  }
  SnoFile<Actor> actor(item->x108_ActorSno.name());
  if (!actor) return std::string();
  return actor->x014_AppearanceSno.name().c_str();
}

Model* Equip(Model& model, char const* hp, char const* id) {
  std::string app = getItemAppearance(id);
  if (!app.empty()) {
    return ShowModel(model.attach(hp, app.c_str()));
  } else {
    SnoFile<Actor> actor(id);
    if (actor) return ShowModel(model.attach(hp, actor->x014_AppearanceSno.name().c_str()));
    else return ShowModel(model.attach(hp, id));
  }
  return nullptr;
}

std::map<std::string, uint32> get_item_icons(bool leg = false);
void dump_data(std::string const& suffix);
void make_menu();
void make_diffs();
void dump_affixes();
void archon();

template<class T>
void SnoMerge(std::map<uint32, std::string>& dst) {
  auto& src = SnoManager::get<T>().get();
  for (auto& kv : src) {
    dst[kv.first] = kv.second;
  }
}
std::map<uint32, std::string> SuperSno() {
  std::map<uint32, std::string> res;
  SnoMerge<Accolade>(res);
  SnoMerge<Act>(res);
  SnoMerge<Actor>(res);
  SnoMerge<Adventure>(res);
  SnoMerge<AmbientSound>(res);
  SnoMerge<Anim>(res);
  SnoMerge<Anim2D>(res);
  SnoMerge<AnimSet>(res);
  SnoMerge<Appearance>(res);
  SnoMerge<BossEncounter>(res);
  SnoMerge<Cloth>(res);
  SnoMerge<Condition>(res);
  SnoMerge<Conversation>(res);
  SnoMerge<EffectGroup>(res);
  SnoMerge<Encounter>(res);
  SnoMerge<Explosion>(res);
  SnoMerge<Font>(res);
  SnoMerge<GameBalance>(res);
  SnoMerge<Globals>(res);
  SnoMerge<LevelArea>(res);
  SnoMerge<Light>(res);
  SnoMerge<Lore>(res);
  SnoMerge<MarkerSet>(res);
  SnoMerge<Material>(res);
  SnoMerge<Monster>(res);
  SnoMerge<Music>(res);
  SnoMerge<Observer>(res);
  SnoMerge<Particle>(res);
  SnoMerge<Physics>(res);
  SnoMerge<PhysMesh>(res);
  SnoMerge<Power>(res);
  SnoMerge<Quest>(res);
  SnoMerge<QuestRange>(res);
  SnoMerge<Recipe>(res);
  SnoMerge<Reverb>(res);
  SnoMerge<Rope>(res);
  SnoMerge<Scene>(res);
  SnoMerge<SceneGroup>(res);
  SnoMerge<ShaderMap>(res);
  SnoMerge<Shaders>(res);
  SnoMerge<Shakes>(res);
  SnoMerge<SkillKit>(res);
  SnoMerge<Sound>(res);
  SnoMerge<SoundBank>(res);
  SnoMerge<Surface>(res);
  SnoMerge<Textures>(res);
  SnoMerge<Trail>(res);
  SnoMerge<Tutorial>(res);
  SnoMerge<UI>(res);
  SnoMerge<Weather>(res);
  SnoMerge<Worlds>(res);
  SnoMerge<StringList>(res);
  return res;
}

void write_tex(std::string const& name, ImageFormat::Type format = ImageFormat::PNG) {
  SnoFile<Textures> tex(name);
  tex->load(0).write("Tex/" + name + ".png", format);
}
void item_flavor(SnoLoader* loader = SnoLoader::default);
void translate_extra();

std::string ActorApp(uint32 aid) {
  SnoFile<Actor> actor(Actor::name(aid));
  if (!actor) return std::string();
  return actor->x014_AppearanceSno.name();
}

std::string ItemModel(std::string const& name) {
  auto* item = ItemLibrary::get(name);
  SnoFile<Actor> actor(item->x108_ActorSno.name());
  if (!actor) return "";
  return actor->x014_AppearanceSno.name();
}
void dump_animsets();
void progress(std::string const& suffix);
void progress_comp(bool trans);
void write_skill_anim();

int do_main() {
  SnoCascLoader casc(path::casc(), "enUS");
  //SnoCascLoader casc("G:\\D3Live\\Data", "enUS");
  SnoLoader::default = &casc;
  WebGL::AllItems(true, false, false);
  //WebGL::Archive lhs(File("textures2.wgz"), false), rhs(File("textures.wgz"), false);
  //WebGL::Archive::compare(File("texturediff.txt", "w"), lhs, rhs, Textures::name);
  return 0;

  //Model model("Monk_Female");
  //model.setAnimation("Monk_Female_DW_SS_Idle_01");
  //Equip(model, "HP_rightWeapon", "Unique_Sword_1H_021_x1");
  //Equip(model, "HP_leftWeapon", "Unique_Sword_1H_101_x1");
  //Equip(model, "HP_Head", "x1_SpiritStone_monkF_norm_unique_16");
  //auto* item = ItemLibrary::get("Unique_Shoulder_Set_02_p3");
  //SnoFile<Actor> metaActor(item->x108_ActorSno.name());
  //std::map<uint32, uint32> tagMap;
  //for (uint32 i = 0; i < metaActor->x060_TagMap[0]; ++i) {
  //  tagMap[metaActor->x060_TagMap[i * 3 + 2]] = metaActor->x060_TagMap[i * 3 + 3];
  //}
  //SnoFile<Actor> actor(Actor::name(tagMap[94720 + 4]));
  //SnoFile<AnimSet> animSet(actor->x068_AnimSetSno.name());
  //uint32 tag = 0;
  //for (auto& maps : animSet->x010_AnimSetTagMaps) {
  //  for (uint32 i = 0; i < maps.x08_TagMap[0]; ++i) {
  //    tag = maps.x08_TagMap[i * 3 + 3];
  //    if (Anim::name(tag)) break;
  //  }
  //  if (Anim::name(tag)) break;
  //}
  //SnoFile<Actor> actor(Actor::name(368562));
  //casc.dump<Appearance>(actor->x014_AppearanceSno.name());
  //return 0;
  Model model("Monk_Male");
  //model.setAnimation(Anim::name(440419));
  //model.appearance(0, 0);
  //Model model(ActorApp(6485).c_str());
  //model.setAnimation(Anim::name(220044));
  //model.appearance(4, 0);
  //model.appearance(8, 0);
  //model.appearance(12, 0);
  //model.appearance(19, 0);
  //model.appearance(20, 0);
  //Model* sub = model.attach("hp_right_shoulderpad", ActorApp(440417).c_str());
  //sub->setAnimation(Anim::name(440419));
  //sub->appearance(0, 0);
  //model.setAnimation("Wizard_Female_HTH_recall_channel");
  //model.appearance(14, 81);
  //model.appearance(10, 81);
  //model.appearance(19, 81);
  //model.appearance(6, 81);
  //model.appearance(3, 0);
  //Equip(model, "HP_rightWeapon", "Unique_Wand_101_x1");
  //Equip(model, "HP_leftWeapon", Actor::name(184199));// ->setAnimation("orb_norm_base_rotateOrb_idle_01");
  //Equip(model, "HP_Head", "x1_SpiritStone_monkF_norm_unique_16");
  //if (File cache = "model.json") {
  //  json::Value value;
  //  json::parse(cache, value);
  //  model.restore(value);
  //}
  ViewModel(model);
  json::Value value;
  model.store(value);
  json::write(File("model.json", "w"), value);

  return 0;
}

int main() {
  std::setlocale(LC_ALL, "eu_US.UTF-8");
  try {
    return do_main();
  } catch (Exception& ex) {
    fprintf(stderr, "Exception: %s\n", ex.what());
    return 1;
  }
}
