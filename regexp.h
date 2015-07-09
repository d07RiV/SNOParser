#pragma once

#include "types.h"
#include <string>
#include <vector>

namespace re {

typedef bool(*CharTraitFunc)(uint32);
class CharacterClass {
  bool invert;
  std::vector<CharTraitFunc> funcs;
  struct Range {
    uint32 begin;
    uint32 end;
    Range(uint32 a = 0, uint32 b = 0)
      : begin(a), end(b)
    {}
  };
  friend bool operator < (Range const& lhs, Range const& rhs) {
    return lhs.begin < rhs.begin;
  }
  std::vector<Range> data;
  void addRange(uint32 a, uint32 b) {
    data.emplace_back(a, b);
  }
  void addClass(CharacterClass const& cls);
  void sort();
public:
  CharacterClass()
    : invert(false)
  {}
  CharacterClass(CharTraitFunc func, bool inv = false)
    : invert(inv)
  {
    funcs.push_back(func);
  }
  CharacterClass(char const* src, int flags = 0) {
    init(src, flags);
  }

  uint8_const_ptr init(char const* src, int flags = 0);

  std::string format() const;

  bool match(uint32 c) const;

  static CharacterClass* getDefault(char ch, int flags = 0);
};

struct Match {
  char const* start[32];
  char const* end[32];
  std::string group(int index) const {
    return std::string(start[index], end[index] - start[index]);
  }
};
struct Thread;
struct State;

class Prog {
  uint32 flags;
  State* start;
  State* states;
  int numStates;
  std::vector<CharacterClass*> masks;
  int numCaptures;

  int maxThreads;
  Thread* threads;
  int cur;
  int numThreads[2];
  char const* matchText;
  void addthread(State* state, Match const& match);
  void advance(State* state, Match const& match, uint32 cp, char const* ref);

  friend struct FindStruct;
  struct FindFunc {
    virtual void call(Match const& match) = 0;
  };
  template<class Func>
  class FindFuncHolder : public FindFunc {
  public:
    FindFuncHolder(Func const& func) : func_(func) {}
    void call(Match const& match) { func_(match); }
  private:
    Func const& func_;
  };

  friend struct ReplaceStruct;
  struct ReplaceFunc {
    virtual std::string call(Match const& match) = 0;
  };
  template<class Func>
  class ReplaceFuncHolder : public ReplaceFunc {
  public:
    ReplaceFuncHolder(Func const& func) : func_(func) {}
    std::string call(Match const& match) { return func_(match); }
  private:
    Func const& func_;
  };

  void findAll_(char const* text, FindFunc* func);
  std::string replace_(char const* text, ReplaceFunc* func);
public:
  Prog(char const* expr, int length = -1, uint32 flags = 0);
  Prog(std::string const& expr, int length = -1, uint32 flags = 0)
    : Prog(expr.c_str(), length, flags)
  {}
  ~Prog();

  enum {
    CaseInsensitive = 0x01,
    DotAll = 0x02,
    MultiLine = 0x04,
    Unicode = 0x08,
  };

  bool match(char const* text, std::vector<std::string>* sub = NULL);
  int find(char const* text, int start = 0, std::vector<std::string>* sub = NULL);

  bool match(std::string const& text, std::vector<std::string>* sub = NULL) {
    return match(text.c_str(), sub);
  }
  int find(std::string const& text, int start = 0, std::vector<std::string>* sub = NULL) {
    return find(text.c_str(), start, sub);
  }

  std::vector<std::string> findAll(char const* text);
  template<class Func>
  void findAll(char const* text, Func const& func) {
    findAll_(text, &FindFuncHolder<Func>(func));
  }

  std::vector<std::string> findAll(std::string const& text) {
    return findAll(text.c_str());
  }
  template<class Func>
  void findAll(std::string const& text, Func const& func) {
    findAll_(text.c_str(), &FindFuncHolder<Func>(func));
  }

  template<class Func>
  std::string replace(char const* text, Func const& func) {
    return replace_(text, &ReplaceFuncHolder<Func>(func));
  }
  std::string replace(char const* text, char const* with);

  template<class Func>
  std::string replace(std::string const& text, Func const& func) {
    return replace_(text.c_str(), &ReplaceFuncHolder<Func>(func));
  }
  std::string replace(std::string const& text, char const* with) {
    return replace(text.c_str(), with);
  }

  int captures() const {
    return numCaptures;
  }
  int run(char const* text, int length, bool exact, bool(*callback) (Match const& match, void* arg), void* arg);
};

}
