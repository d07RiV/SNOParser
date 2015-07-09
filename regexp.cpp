#include <string.h>
#include <algorithm>

#include "regexp.h"
#include "utf8.h"
#include "path.h"

namespace re {

static uint32 unescape(uint8_const_ptr& chr) {
  uint32 cp = 0;
  switch (*chr) {
  case 'a': chr++; return '\a';
  case 'b': chr++; return '\b';
  case 'f': chr++; return '\f';
  case 'n': chr++; return '\n';
  case 'r': chr++; return '\r';
  case 't': chr++; return '\t';
  case 'v': chr++; return '\v';
  case 'u':
    chr++;
    for (int z = 0; z < 4 && isxdigit(*chr); z++) {
      cp *= 16;
      if (*chr >= '0' && *chr <= '9') cp += int(*chr - '0');
      else if (*chr >= 'a' && *chr <= 'f') cp += 10 + int(*chr - 'a');
      else if (*chr >= 'A' && *chr <= 'F') cp += 10 + int(*chr - 'A');
      chr++;
    }
    return cp;
  default:
    if (isdigit(*chr)) {
      for (int z = 0; z < 3 && isdigit(*chr); z++)
        cp = cp * 10 + int((*chr++) - '0');
      return cp;
    } else {
      return utf8::parse(utf8::transform(&chr, NULL));
    }
  }
}
static uint32 getchar(uint8_const_ptr& chr, uint32* table = NULL) {
  if (*chr == '\\') return unescape(++chr);
  return utf8::parse(utf8::transform(&chr, table));
}

void CharacterClass::sort() {
  std::sort(data.begin(), data.end());
  uint32 shift = 0;
  for (uint32 i = 0; i < data.size(); i++) {
    if (data[i].end < data[i].begin || (i - shift > 0 && data[i].end <= data[i - shift - 1].end)) {
      shift++;
    } else if (i - shift > 0 && data[i].begin <= data[i - shift - 1].end + 1) {
      data[i - shift - 1].end = data[i].end;
      shift++;
    } else if (shift) {
      data[i - shift] = data[i];
    }
  }
  data.resize(data.size() - shift);
}

static void _addchar(std::string& res, char chr) {
  if (chr == ']' || chr == '-' || chr == '\\' || chr == '^' || chr == '[') {
    res.push_back('\\');
  }
  res.push_back(chr);
}

std::string CharacterClass::format() const {
  bool chrmap[256];
  int has = 0, not = 0;
  for (uint32 cp = 32; cp <= 126; ++cp) {
    chrmap[cp] = match(cp);
    has += (chrmap[cp] == true);
    not += (chrmap[cp] == false);
  }
  if (!not) return ".";
  std::string res("[");
  if (has > not) {
    res.push_back('^');
    for (uint32 cp = 32; cp <= 126; ++cp) {
      chrmap[cp] = !chrmap[cp];
    }
  }
  for (uint32 cp = 32; cp <= 126; ++cp) {
    if (chrmap[cp]) {
      uint32 start = cp;
      while (cp < 126 && chrmap[cp + 1]) {
        ++cp;
      }
      if (cp > start + 2) {
        _addchar(res, start);
        res.push_back('-');
        _addchar(res, cp);
      } else {
        for (uint32 i = start; i <= cp; ++i) {
          _addchar(res, i);
        }
      }
    }
  }
  res.push_back(']');
  return res;
}

void CharacterClass::addClass(CharacterClass const& cls) {
  funcs.insert(funcs.end(), cls.funcs.begin(), cls.funcs.end());
  if (cls.invert) {
    if (cls.data.size() == 0) {
      addRange(0, 0xFFFFFFFF);
    } else {
      if (cls.data[0].begin > 0) {
        addRange(0, cls.data[0].begin - 1);
      }
      for (int i = 1; i < cls.data.size(); i++) {
        addRange(cls.data[i - 1].end + 1, cls.data[i].begin - 1);
      }
      if (cls.data[cls.data.size() - 1].end < 0xFFFFFFFF) {
        addRange(cls.data[cls.data.size() - 1].end + 1, 0xFFFFFFFF);
      }
    }
  } else {
    data.insert(data.end(), cls.data.begin(), cls.data.end());
  }
}
uint8_const_ptr CharacterClass::init(char const* src, int flags) {
  invert = (*src == '^');
  if (invert) src++;
  uint8_const_ptr pos = (uint8_const_ptr)src;
  uint32* table = ((flags & Prog::CaseInsensitive) ? utf8::tf_lower : NULL);
  while (*pos && (pos == (uint8_const_ptr)src || *pos != ']')) {
    uint32 cp = -1;
    if (*pos == '\\') {
      pos++;
      CharacterClass* cls = getDefault(*pos, flags);
      if (cls) {
        pos++;
        addClass(*cls);
      } else {
        cp = unescape(pos);
        if (flags & Prog::CaseInsensitive) cp = towlower(cp);
      }
    } else {
      cp = utf8::parse(utf8::transform(&pos, table));
    }
    if (cp != -1) {
      if (*pos == '-' && pos[1] && pos[1] != ']') {
        pos++;
        addRange(cp, getchar(pos, table));
      } else {
        addRange(cp, cp);
      }
    }
  }
  sort();
  return pos;
}
bool CharacterClass::match(uint32 c) const {
  int left = 0;
  int right = data.size() - 1;
  while (left <= right) {
    int mid = (left + right) / 2;
    if (c >= data[mid].begin && c <= data[mid].end) {
      return !invert;
    }
    if (c < data[mid].begin) {
      right = mid - 1;
    } else {
      left = mid + 1;
    }
  }
  for (int i = 0; i < funcs.size(); i++) {
    if (funcs[i](c)) return !invert;
  }
  return invert;
}

static CharacterClass uClsNormal[] = {
  CharacterClass("a-zA-Z0-9_"),
  CharacterClass("^a-zA-Z0-9_"),
  CharacterClass("0-9"),
  CharacterClass("a-fA-F0-9"),
  CharacterClass("^0-9"),
  CharacterClass(" \t\n\r\f\v"),
  CharacterClass("^ \t\n\r\f\v"),
  CharacterClass("^\n"),
  CharacterClass("^"),
};

static bool u_word(uint32 cp) {
  return cp == '_' || (cp >= '0' && cp <= '9') || iswalnum(cp);
}
static bool u_notword(uint32 cp) {
  return !(cp == '_' || (cp >= '0' && cp <= '9') || iswalnum(cp));
}
static bool u_space(uint32 cp) {
  return iswspace(cp);
}
static bool u_notspace(uint32 cp) {
  return !iswspace(cp);
}
static CharacterClass uClsUnicode[] = {
  CharacterClass(u_word),
  CharacterClass(u_notword),
  CharacterClass(u_space),
  CharacterClass(u_notspace),
};

CharacterClass* CharacterClass::getDefault(char ch, int flags) {
  switch (ch) {
  case 'w': return (flags & Prog::Unicode ? &uClsUnicode[0] : &uClsNormal[0]);
  case 'W': return (flags & Prog::Unicode ? &uClsUnicode[1] : &uClsNormal[1]);
  case 'd': return &uClsNormal[2];
  case 'x': return &uClsNormal[3];
  case 'D': return &uClsNormal[4];
  case 's': return (flags & Prog::Unicode ? &uClsUnicode[2] : &uClsNormal[5]);
  case 'S': return (flags & Prog::Unicode ? &uClsUnicode[3] : &uClsNormal[6]);
  case '.': return (flags & Prog::DotAll ? &uClsNormal[8] : &uClsNormal[7]);
  default: return NULL;
  }
}

/////////////////////////////////////

struct State {
  enum Type {
    NONE = 0,
    START = 1,
    RBRA,
    RBRANC,
    LBRA,
    LBRANC,
    OR,
    CAT,
    STAR,
    PLUS,
    QUEST,
    NOP,
    OPERAND,
    BOL,
    EOL,
    CHAR,
    CCLASS,
    END
  };
  Type type;
  union {
    CharacterClass const* mask;
    uint32 chr;
    int subid;
    State* left;
  };
  union {
    State* right;
    State* next;
  };
  int list;
};
struct Operand {
  State* first;
  State* last;
};
struct Operator {
  State::Type type;
  int subid;
};
#define MAXSTACK    32
struct Compiler {
  State* states;
  int maxStates;
  int numStates;

  Operand andstack[MAXSTACK];
  int andsize;
  Operator atorstack[MAXSTACK];
  int atorsize;

  bool lastand;
  int brackets;
  int cursub;

  int optsize;

  void init(char const* expr, int len);
  State* operand(State::Type type);
  void pushand(State* first, State* last);
  void pushator(State::Type type);
  void evaluntil(int pri);
  int optimize(State* state);
  void floatstart();
};
void Compiler::init(char const* expr, int length) {
  maxStates = length * 6 + 6;
  states = new State[maxStates];
  memset(states, 0, sizeof(State)* maxStates);
  numStates = 0;

  andsize = 0;
  atorstack[0].type = State::NONE;
  atorstack[0].subid = 0;
  atorsize = 1;
  lastand = false;
  brackets = 0;
  cursub = 0;

  optsize = 0;
}
State* Compiler::operand(State::Type type) {
  if (lastand) pushator(State::CAT);
  State* s = &states[numStates++];
  s->type = type;
  s->mask = NULL;
  s->next = NULL;
  pushand(s, s);
  lastand = true;
  return s;
}
void Compiler::pushand(State* first, State* last) {
  andstack[andsize].first = first;
  andstack[andsize].last = last;
  andsize++;
}
void Compiler::pushator(State::Type type) {
  if (type == State::RBRA || type == State::RBRANC) {
    brackets--;
  }
  if (type == State::LBRA || type == State::LBRANC) {
    if (type == State::LBRA) cursub++;
    brackets++;
    if (lastand) pushator(State::CAT);
  } else {
    evaluntil(type);
  }
  if (type != State::RBRA && type != State::RBRANC) {
    atorstack[atorsize].type = type;
    atorstack[atorsize].subid = cursub;
    atorsize++;
  }
  lastand = (type == State::STAR || type == State::PLUS ||
             type == State::QUEST || type == State::RBRA || type == State::RBRANC);
}
void Compiler::evaluntil(int pri) {
  State* s1;
  State* s2;
  while (pri == State::RBRA || pri == State::RBRANC || atorstack[atorsize - 1].type >= pri) {
    atorsize--;
    switch (atorstack[atorsize].type) {
    case State::LBRA:
      s1 = &states[numStates++];
      s2 = &states[numStates++];
      s1->type = State::LBRA;
      s1->subid = atorstack[atorsize].subid;
      s2->type = State::RBRA;
      s2->subid = atorstack[atorsize].subid;

      s1->next = andstack[andsize - 1].first;
      andstack[andsize - 1].first = s1;
      andstack[andsize - 1].last->next = s2;
      andstack[andsize - 1].last = s2;
      s2->next = NULL;
      return;
    case State::LBRANC:
      return;
    case State::OR:
      andsize--;
      s1 = &states[numStates++];
      s2 = &states[numStates++];
      s1->type = State::OR;
      s1->left = andstack[andsize - 1].first;
      s1->right = andstack[andsize].first;
      s2->type = State::NOP;
      s2->mask = NULL;
      s2->next = NULL;
      andstack[andsize - 1].last->next = s2;
      andstack[andsize].last->next = s2;
      andstack[andsize - 1].first = s1;
      andstack[andsize - 1].last = s2;
      break;
    case State::CAT:
      andsize--;
      andstack[andsize - 1].last->next = andstack[andsize].first;
      andstack[andsize - 1].last = andstack[andsize].last;
      break;
    case State::STAR:
      s1 = &states[numStates++];
      s1->type = State::OR;
      s1->left = andstack[andsize - 1].first;
      s1->right = NULL;
      andstack[andsize - 1].last->next = s1;
      andstack[andsize - 1].first = s1;
      andstack[andsize - 1].last = s1;
      break;
    case State::PLUS:
      s1 = &states[numStates++];
      s1->type = State::OR;
      s1->left = andstack[andsize - 1].first;
      s1->right = NULL;
      andstack[andsize - 1].last->next = s1;
      andstack[andsize - 1].last = s1;
      break;
    case State::QUEST:
      s1 = &states[numStates++];
      s2 = &states[numStates++];
      s1->type = State::OR;
      s1->left = andstack[andsize - 1].first;
      s1->right = s2;
      s2->type = State::NOP;
      s2->mask = NULL;
      s2->next = NULL;
      andstack[andsize - 1].last->next = s2;
      andstack[andsize - 1].first = s1;
      andstack[andsize - 1].last = s2;
      break;
    }
  }
}
int Compiler::optimize(State* state) {
  if (state->list >= 0) return state->list;
  if (state->type == State::NOP) {
    state->list = optimize(state->next);
  } else {
    state->list = optsize++;
    if (state->next)
      optimize(state->next);
    if (state->type == State::OR)
      optimize(state->left);
  }
  return state->list;
}

struct Thread {
  State* state;
  Match match;
};

Prog::Prog(char const* expr, int length, uint32 f) {
  flags = f;
  Compiler comp;
  if (length < 0) length = strlen(expr);
  comp.init(expr, length);

  uint32* ut_table = (flags & CaseInsensitive ? utf8::tf_lower : NULL);

  uint8_const_ptr pos = (uint8_const_ptr)expr;
  uint8_const_ptr end = pos + length;
  while (pos < end) {
    State::Type type = State::CHAR;
    CharacterClass const* mask = NULL;
    uint32 cp = -1;
    switch (*pos) {
    case '\\':
      pos++;
      if (*pos != '.' && (mask = CharacterClass::getDefault(*pos, flags))) {
        type = State::CCLASS;
      } else {
        cp = unescape(pos);
        if (flags & CaseInsensitive) cp = towlower(cp);
        pos--;
      }
      break;
    case '*':
      type = State::STAR;
      break;
    case '+':
      type = State::PLUS;
      break;
    case '?':
      type = State::QUEST;
      break;
    case '|':
      type = State::OR;
      break;
    case '(':
      type = State::LBRA;
      break;
    case ')':
      type = State::RBRA;
      break;
    case '{':
      type = State::LBRANC;
      break;
    case '}':
      type = State::RBRANC;
      break;
    case '^':
      type = State::BOL;
      break;
    case '$':
      type = State::EOL;
      break;
    case '.':
      type = State::CCLASS;
      mask = CharacterClass::getDefault('.', flags);
      break;
    case '[': {
      type = State::CCLASS;
      CharacterClass* cls = new CharacterClass();
      masks.push_back(cls);
      pos = cls->init((char*)pos + 1, flags);
      mask = cls;
      break;
    }
    }
    if (cp == -1) {
      cp = utf8::parse(utf8::transform(&pos, ut_table));
    } else {
      pos++;
    }
    if (type < State::OPERAND) {
      comp.pushator(type);
    } else {
      State* s = comp.operand(type);
      if (type == State::CHAR) {
        s->chr = cp;
      } else if (type == State::CCLASS) {
        s->mask = mask;
      }
    }
  }
  comp.evaluntil(State::START);
  comp.operand(State::END);
  comp.evaluntil(State::START);

  for (int i = 0; i < comp.numStates; i++) {
    comp.states[i].list = -1;
  }
  int startpos = comp.optimize(comp.andstack[0].first);
  states = new State[comp.optsize];
  for (int i = 0; i < comp.numStates; i++) {
    int p = comp.states[i].list;
    if (p >= 0 && comp.states[i].type != State::NOP) {
      states[p].type = comp.states[i].type;
      states[p].next = comp.states[i].next ? &states[comp.states[i].next->list] : NULL;
      if (states[p].type == State::CHAR) {
        states[p].chr = comp.states[i].chr;
      } else if (states[p].type == State::CCLASS) {
        states[p].mask = comp.states[i].mask;
      } else if (states[p].type == State::OR) {
        states[p].left = comp.states[i].left ? &states[comp.states[i].left->list] : NULL;
      } else {
        states[p].subid = comp.states[i].subid;
      }
    }
  }
  delete[] comp.states;
  numStates = comp.optsize;
  start = &states[startpos];

  numCaptures = comp.cursub;
  maxThreads = 32;
  threads = new Thread[maxThreads * 2];
}
Prog::~Prog() {
  delete[] states;
  for (int i = 0; i < masks.size(); i++) {
    delete masks[i];
  }
  delete[] threads;
}
void Prog::addthread(State* state, Match const& match) {
  if (state->list < 0) {
    if (numThreads[1 - cur] >= maxThreads) {
      Thread* newthreads = new Thread[maxThreads * 4];
      memcpy(newthreads, threads, sizeof(Thread)* numThreads[0]);
      memcpy(newthreads + maxThreads * 2, threads + maxThreads, sizeof(Thread)* numThreads[1]);
      delete[] threads;
      threads = newthreads;
      maxThreads *= 2;
    }
    Thread* thread = &threads[(1 - cur) * maxThreads + numThreads[1 - cur]];
    state->list = numThreads[1 - cur];
    numThreads[1 - cur]++;
    thread->state = state;
    memcpy(&thread->match, &match, sizeof match);
  }
}
void Prog::advance(State* state, Match const& match, uint32 cp, char const* ref) {
  if (state->type == State::OR) {
    advance(state->left, match, cp, ref);
    advance(state->right, match, cp, ref);
  } else if (state->type == State::LBRA) {
    Match m2;
    memcpy(&m2, &match, sizeof m2);
    m2.start[state->subid] = ref;
    advance(state->next, m2, cp, ref);
  } else if (state->type == State::RBRA) {
    Match m2;
    memcpy(&m2, &match, sizeof m2);
    m2.end[state->subid] = ref;
    advance(state->next, m2, cp, ref);
  } else if (state->type == State::BOL) {
    if (flags & MultiLine) {
      if (ref == matchText || ref[-1] == '\n') {
        advance(state->next, match, 0xFFFFFFFF, ref);
      }
    } else {
      if (ref == matchText) {
        advance(state->next, match, 0xFFFFFFFF, ref);
      }
    }
  } else if (state->type == State::EOL) {
    if (flags & MultiLine) {
      if (*ref == 0 || *ref == '\r' || *ref == '\n') {
        advance(state->next, match, 0xFFFFFFFF, ref);
      }
    } else {
      if (*ref == 0) {
        advance(state->next, match, 0xFFFFFFFF, ref);
      }
    }
  } else {
    if (cp == 0xFFFFFFFF) {
      addthread(state, match);
    }
    else if (cp && ((state->type == State::CHAR && cp == state->chr) ||
             (state->type == State::CCLASS && state->mask->match(cp)))) {
      advance(state->next, match, 0xFFFFFFFF, (char*)utf8::next((uint8_const_ptr)ref));
    }
  }
}
int Prog::run(char const* text, int length, bool exact,
              bool(*callback) (Match const& match, void* arg), void* arg) {
  cur = 0;
  numThreads[0] = 0;
  numThreads[1] = 0;
  int pos = 0;
  if (length < 0) length = strlen(text);
  matchText = text;
  int count = 0;
  uint32* ut_table = (flags & CaseInsensitive ? utf8::tf_lower : NULL);

  while (true) {
    for (int i = 0; i < numStates; i++) {
      if (pos > 0 && states[i].type == State::END && states[i].list >= 0 &&
          (!exact || pos == length)) {
        Thread* thread = &threads[cur * maxThreads + states[i].list];
        thread->match.end[0] = text + pos;
        count++;
        if (callback) {
          if (!callback(thread->match, arg)) return count;
        }
      }
      states[i].list = -1;
    }
    numThreads[1 - cur] = 0;
    uint8_const_ptr next = (uint8_const_ptr)(text + pos);
    uint32 cp = utf8::parse(utf8::transform(&next, ut_table));
    if (cp == '\r' && *next == '\n') next++;
    for (int i = 0; i < numThreads[cur]; i++) {
      advance(threads[cur * maxThreads + i].state, threads[cur * maxThreads + i].match, cp, text + pos);
    }
    if (pos == 0 || !exact) {
      Match match;
      memset(&match, 0, sizeof match);
      match.start[0] = text + pos;
      advance(start, match, cp, text + pos);
    }
    cur = 1 - cur;
    if (pos >= length) break;
    pos = (char*)next - text;
  }
  for (int i = 0; i < numStates; i++) {
    if (states[i].type == State::END && states[i].list >= 0) {
      Thread* thread = &threads[cur * maxThreads + states[i].list];
      thread->match.end[0] = text + pos;
      count++;
      if (callback) {
        if (!callback(thread->match, arg)) return count;
      }
    }
  }
  return count;
}

static bool matcher(Match const& match, void* arg) {
  if (arg) {
    auto sub = static_cast<std::vector<std::string>*>(arg);
    sub->clear();
    for (int i = 0; i < sizeof(match.start) / sizeof(match.start[0]) && match.start[i]; i++) {
      sub->emplace_back(match.start[i], match.end[i] - match.start[i]);
    }
  }
  return true;
}
bool Prog::match(char const* text, std::vector<std::string>* sub) {
  int res = run(text, -1, true, matcher, sub);
  if (res) {
    if (sub) {
      while (sub->size() <= numCaptures) {
        sub->emplace_back("");
      }
    }
    return true;
  } else {
    return false;
  }
}
static bool finder(Match const& match, void* arg) {
  memcpy(arg, &match, sizeof match);
  return false;
}
int Prog::find(char const* text, int start, std::vector<std::string>* sub)
{
  Match match;
  if (run(text + start, -1, false, finder, &match)) {
    if (sub) {
      sub->clear();
      for (int i = 0; i < sizeof(match.start) / sizeof(match.start[0]) && match.start[i]; i++) {
        sub->emplace_back(match.start[i], match.end[i] - match.start[i]);
      }
      while (sub->size() <= numCaptures) {
        sub->emplace_back("");
      }
    }
    return match.start[0] - text;
  } else {
    return -1;
  }
}

bool operator < (Match const& lhs, Match const& rhs) {
  if (lhs.start[0] != rhs.start[0]) return lhs.start[0] < rhs.start[0];
  return lhs.end[0] > rhs.end[0];
}

struct FindStruct {
  std::vector<Match> matches;
  std::vector<std::string>* result = nullptr;
  Prog::FindFunc* func = nullptr;
  char const* text;

  FindStruct(char const* text)
    : text(text)
  {}
  static bool callback(Match const& match, void* arg);
  void finish();
};
bool FindStruct::callback(Match const& match, void* arg) {
  FindStruct& rs = *(FindStruct*)arg;
  rs.matches.push_back(match);
  return true;
}
void FindStruct::finish() {
  std::sort(matches.begin(), matches.end());
  char const* end = text;
  for (auto& m : matches) {
    if (m.start[0] >= end) {
      if (func) {
        func->call(m);
      } else {
        result->emplace_back(m.start[0], m.end[0] - m.start[0]);
      }
      end = m.end[0];
    }
  }
}
std::vector<std::string> Prog::findAll(char const* text) {
  std::vector<std::string> result;
  FindStruct rs(text);
  rs.result = &result;
  run(text, -1, false, FindStruct::callback, &rs);
  rs.finish();
  return result;
}
void Prog::findAll_(char const* text, FindFunc* func) {
  FindStruct rs(text);
  rs.func = func;
  run(text, -1, false, FindStruct::callback, &rs);
  rs.finish();
}

struct ReplaceStruct {
  std::vector<Match> matches;
  Prog::ReplaceFunc* func = nullptr;
  char const* with = nullptr;
  char const* text;

  ReplaceStruct(char const* text)
    : text(text)
  {}
  static bool callback(Match const& match, void* arg);
  std::string finish();
};
static void addreplace(std::string& result, char const* with, Match const& match) {
  for (int i = 0; with[i]; i++) {
    if (with[i] == '\\') {
      i++;
      if (with[i] >= '0' && with[i] <= '9') {
        int m = int(with[i] - '0');
        if (match.start[m] && match.end[m]) {
          result.append(match.start[m], match.end[m] - match.start[m]);
        }
      } else {
        result.push_back(with[i]);
      }
    } else {
      result.push_back(with[i]);
    }
  }
}
bool ReplaceStruct::callback(Match const& match, void* arg) {
  ReplaceStruct& rs = *(ReplaceStruct*)arg;
  rs.matches.push_back(match);
  return true;
}
std::string ReplaceStruct::finish() {
  std::string result;
  std::sort(matches.begin(), matches.end());
  char const* end = text;
  for (auto& m : matches) {
    if (m.start[0] >= end) {
      result.append(end, m.start[0] - end);
      if (func) {
        result.append(func->call(m));
      } else {
        addreplace(result, with, m);
      }
      end = m.end[0];
    }
  }
  result.append(end);
  return result;
}

std::string Prog::replace(char const* text, char const* with) {
  ReplaceStruct rs(text);
  rs.with = with;
  run(text, -1, false, ReplaceStruct::callback, &rs);
  return rs.finish();
}
std::string Prog::replace_(char const* text, ReplaceFunc* with) {
  ReplaceStruct rs(text);
  rs.func = with;
  run(text, -1, false, ReplaceStruct::callback, &rs);
  return rs.finish();
}

}
