#include "snocommon.h"
#include "snotypes.h"

char const* SNOName::c_type() const {
  static char unk[64];
  switch (group) {
  case 1: return Actor::type();
  case 5: return AmbientSound::type();
  case 14: return EffectGroup::type();
  case 17: return Explosion::type();
  case 23: return Light::type();
  case 27: return Particle::type();
  case 32: return Rope::type();
  case 38: return Shakes::type();
  case 40: return Sound::type();
  case 45: return Trail::type();
  default: return nullptr;
  }
}
char const* SNOName::c_name() const {
  static char unk[64];
  switch (group) {
  case 1: return Actor::name(id);
  case 5: return AmbientSound::name(id);
  case 14: return EffectGroup::name(id);
  case 17: return Explosion::name(id);
  case 23: return Light::name(id);
  case 27: return Particle::name(id);
  case 32: return Rope::name(id);
  case 38: return Shakes::name(id);
  case 40: return Sound::name(id);
  case 45: return Trail::name(id);
  default: return nullptr;
  }
}
