#pragma once

#include "file.h"
#include "json.h"
#include "snomap.h"
#include <vector>

#define DECLARE_SNO(_name, _ext)                              \
  struct _name {                                              \
    static char const* type() { return #_name; }              \
    static char const* ext() { return _ext; }                 \
    static void parse(File& src, json::Visitor* visitor);     \
    static char const* name(uint32 id) {                      \
      return SnoManager::get<_name>()[id];                    \
    }                                                         \
    struct Type;                                              \
  };

DECLARE_SNO(Accolade,       ".aco")
DECLARE_SNO(Act,            ".act")
DECLARE_SNO(Actor,          ".acr")
DECLARE_SNO(Adventure,      ".adv")
DECLARE_SNO(AmbientSound,   ".ams")
DECLARE_SNO(Anim,           ".ani")
DECLARE_SNO(Anim2D,         ".an2")
DECLARE_SNO(AnimSet,        ".ans")
DECLARE_SNO(Appearance,     ".app")
DECLARE_SNO(BossEncounter,  ".bos")
DECLARE_SNO(Cloth,          ".clt")
DECLARE_SNO(Condition,      ".cnd")
DECLARE_SNO(Conversation,   ".cnv")
DECLARE_SNO(EffectGroup,    ".efg")
DECLARE_SNO(Encounter,      ".enc")
DECLARE_SNO(Explosion,      ".xpl")
DECLARE_SNO(Font,           ".fnt")
DECLARE_SNO(GameBalance,    ".gam")
DECLARE_SNO(Globals,        ".glo")
DECLARE_SNO(LevelArea,      ".lvl")
DECLARE_SNO(Light,          ".lit")
DECLARE_SNO(Lore,           ".lor")
DECLARE_SNO(MarkerSet,      ".mrk")
DECLARE_SNO(Material,       ".mat")
DECLARE_SNO(Monster,        ".mon")
DECLARE_SNO(Music,          ".mus")
DECLARE_SNO(Observer,       ".obs")
DECLARE_SNO(Particle,       ".prt")
DECLARE_SNO(Physics,        ".phy")
DECLARE_SNO(PhysMesh,       ".phm")
DECLARE_SNO(Power,          ".pow")
DECLARE_SNO(Quest,          ".qst")
DECLARE_SNO(QuestRange,     ".qsr")
DECLARE_SNO(Recipe,         ".rcp")
DECLARE_SNO(Reverb,         ".rev")
DECLARE_SNO(Rope,           ".rop")
DECLARE_SNO(Scene,          ".scn")
DECLARE_SNO(SceneGroup,     ".scg")
DECLARE_SNO(ShaderMap,      ".shm")
DECLARE_SNO(Shaders,        ".shd")
DECLARE_SNO(Shakes,         ".shk")
DECLARE_SNO(SkillKit,       ".skl")
DECLARE_SNO(Sound,          ".snd")
DECLARE_SNO(SoundBank,      ".sbk")
DECLARE_SNO(Surface,        ".srf")
DECLARE_SNO(Textures,       ".tex")
DECLARE_SNO(Trail,          ".trl")
DECLARE_SNO(Tutorial,       ".tut")
DECLARE_SNO(UI,             ".ui")
DECLARE_SNO(Weather,        ".wth")
DECLARE_SNO(Worlds,         ".wrl")
DECLARE_SNO(StringList,     ".stl")
