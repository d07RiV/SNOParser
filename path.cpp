#include "path.h"
#include "common.h"
#include <windows.h>
using namespace std;

string path::name(string const& path) {
  int pos = path.length();
  while (pos > 0 && path[pos - 1] != '/' && path[pos - 1] != '\\') {
    --pos;
  }
  return path.substr(pos);
}
string path::title(string const& path) {
  int pos = path.length();
  int dot = path.length();
  while (pos > 0 && path[pos - 1] != '/' && path[pos - 1] != '\\') {
    --pos;
    if (path[pos] == '.' && dot == path.length()) {
      dot = pos;
    }
  }
  if (dot == pos) {
    return path.substr(pos);
  } else {
    return path.substr(pos, dot - pos);
  }
}
string path::path(string const& path) {
  int pos = path.length();
  while (pos > 0 && path[pos - 1] != '/' && path[pos - 1] != '\\') {
    --pos;
  }
  return path.substr(0, pos ? pos - 1 : 0);
}
string path::ext(string const& path) {
  int pos = path.length();
  int dot = path.length();
  while (pos > 0 && path[pos - 1] != '/' && path[pos - 1] != '\\') {
    --pos;
    if (path[pos] == '.' && dot == path.length()) {
      dot = pos;
    }
  }
  if (dot == pos) {
    return "";
  } else {
    return path.substr(dot);
  }
}

namespace path {
  struct Paths {
    string root, work, casc;
    static Paths& get();
  };
  bool initialized = false;

  Paths& Paths::get() {
    static std::string roots[] = {
      "C:\\Work\\junk",
      "C:\\Webroot\\game",
      "C:\\tmp",
    };
    static std::string cascs[] = {
      "E:\\D3Ptr\\Data",
    };
    static bool initialized = false;
    static Paths instance;
    if (initialized) return instance;
    for (auto const& root : roots) {
      uint32 attr = GetFileAttributes(root.c_str());
      if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
        instance.root = root / "Base";
        instance.work = root / "Work";
        SetCurrentDirectory(instance.work.c_str());
        initialized = true;
        break;
      }
    }
    if (!initialized) throw Exception("work dir not found");
    initialized = false;
    for (auto const& casc : cascs) {
      uint32 attr = GetFileAttributes(casc.c_str());
      if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
        instance.casc = casc;
        initialized = true;
        break;
      }
    }
    if (!initialized) throw Exception("casc dir not found");
    return instance;
  }
}

std::string const& path::root() {
  return Paths::get().root;
}
std::string const& path::work() {
  return Paths::get().work;
}
std::string const& path::casc() {
  return Paths::get().casc;
}

string operator / (string const& lhs, string const& rhs) {
  if (lhs.empty() || rhs.empty()) return lhs + rhs;
  bool left = (lhs.back() == '\\' || lhs.back() == '/');
  bool right = (rhs.front() == '\\' || rhs.front() == '/');
  if (left && right) {
    string res = lhs;
    res.pop_back();
    return res + rhs;
  } else if (!left && !right) {
    return lhs + path::sep + rhs;
  } else {
    return lhs + rhs;
  }
}
