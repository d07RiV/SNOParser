#include "snocommon.h"
#include "snotypes.h"

size_t FindSnoSize(char const* type, size_t size) {
  return size;
}

char const* SNOName::c_type() const {
  static char unk[64];
#define SNOTYPE(T)   case T::index: return T::type();
  switch (group) {
#include "allsno.h"
  default: return nullptr;
  }
#undef SNOTYPE
}
char const* SNOName::c_name() const {
  static char unk[64];
#define SNOTYPE(T)   case T::index: return T::name(id);
  switch (group) {
#include "allsno.h"
  default: return nullptr;
  }
#undef SNOTYPE
}
