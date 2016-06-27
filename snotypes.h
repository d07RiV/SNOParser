#pragma once

#include "file.h"
#include "json.h"
#include "snomap.h"
#include <vector>

#define DECLARE_SNO(_name, _ext, _idx)                        \
  struct _name {                                              \
    static char const* type() { return #_name; }              \
    static char const* ext() { return _ext; }                 \
    static const uint32 index = _idx;                         \
    static void parse(File& src, json::Visitor* visitor);     \
    static char const* name(uint32 id) {                      \
      return SnoManager::get<_name>()[id];                    \
    }                                                         \
    struct Type;                                              \
    static SnoInfo const& info() {                            \
      static SnoInfo res = {type(), ext(), index};            \
      return res;                                             \
    }                                                         \
  };

DECLARE_SNO(Accolade,       ".aco", 0x42)
DECLARE_SNO(Act,            ".act", 0x38)
DECLARE_SNO(Actor,          ".acr", 0x01)
DECLARE_SNO(Adventure,      ".adv", 0x02)
DECLARE_SNO(AmbientSound,   ".ams", 0x05)
DECLARE_SNO(Anim,           ".ani", 0x06)
DECLARE_SNO(Anim2D,         ".an2", 0x07)
DECLARE_SNO(AnimSet,        ".ans", 0x08)
DECLARE_SNO(Appearance,     ".app", 0x09)
DECLARE_SNO(BossEncounter,  ".bos", 0x40)
DECLARE_SNO(Cloth,          ".clt", 0x0B)
DECLARE_SNO(Condition,      ".cnd", 0x33)
DECLARE_SNO(Conversation,   ".cnv", 0x0C)
DECLARE_SNO(EffectGroup,    ".efg", 0x0E)
DECLARE_SNO(Encounter,      ".enc", 0x0F)
DECLARE_SNO(Explosion,      ".xpl", 0x11)
DECLARE_SNO(Font,           ".fnt", 0x13)
DECLARE_SNO(GameBalance,    ".gam", 0x14)
DECLARE_SNO(Globals,        ".glo", 0x15)
DECLARE_SNO(LevelArea,      ".lvl", 0x16)
DECLARE_SNO(Light,          ".lit", 0x17)
DECLARE_SNO(Lore,           ".lor", 0x3B)
DECLARE_SNO(MarkerSet,      ".mrk", 0x18)
DECLARE_SNO(Material,       ".mat", 0x39)
DECLARE_SNO(Monster,        ".mon", 0x19)
DECLARE_SNO(Music,          ".mus", 0x3E)
DECLARE_SNO(Observer,       ".obs", 0x1A)
DECLARE_SNO(Particle,       ".prt", 0x1B)
DECLARE_SNO(Physics,        ".phy", 0x1C)
DECLARE_SNO(PhysMesh,       ".phm", 0x3D)
DECLARE_SNO(Power,          ".pow", 0x1D)
DECLARE_SNO(Quest,          ".qst", 0x1F)
DECLARE_SNO(QuestRange,     ".qsr", 0x3A)
DECLARE_SNO(Recipe,         ".rcp", 0x31)
DECLARE_SNO(Reverb,         ".rev", 0x3C)
DECLARE_SNO(Rope,           ".rop", 0x20)
DECLARE_SNO(Scene,          ".scn", 0x21)
DECLARE_SNO(SceneGroup,     ".scg", 0x22)
DECLARE_SNO(ShaderMap,      ".shm", 0x24)
DECLARE_SNO(Shaders,        ".shd", 0x25)
DECLARE_SNO(Shakes,         ".shk", 0x26)
DECLARE_SNO(SkillKit,       ".skl", 0x27)
DECLARE_SNO(Sound,          ".snd", 0x28)
DECLARE_SNO(SoundBank,      ".sbk", 0x29)
DECLARE_SNO(Surface,        ".srf", 0x2B)
DECLARE_SNO(Textures,       ".tex", 0x2C)
DECLARE_SNO(Trail,          ".trl", 0x2D)
DECLARE_SNO(Tutorial,       ".tut", 0x3F)
DECLARE_SNO(UI,             ".ui",  0x2E)
DECLARE_SNO(Weather,        ".wth", 0x2F)
DECLARE_SNO(Worlds,         ".wrl", 0x30)
DECLARE_SNO(StringList,     ".stl", 0x2A)
