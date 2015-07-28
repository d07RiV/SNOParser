#pragma once

#include "types.h"
#include <string>
#include <sstream>
#include <cctype>
#include <vector>
#include <map>
#define NOMINMAX
#include <windows.h>

class Exception {
public:
  Exception(char const* fmt, ...);
  Exception(Exception const& e)
    : buf_(e.buf_.str())
  {}

  virtual char const* what() const throw() {
    str_ = buf_.str();
    return str_.c_str();
  }

  template<class T>
  inline void append(T const& t) {
    buf_ << t;
  }
private:
  mutable std::string str_;
  std::stringstream buf_;
};

template<class T>
static inline Exception&& operator<<(Exception&& e, T const& t) {
  e.append(t);
  return std::forward<Exception>(e);
}

std::string fmtstring(char const* fmt, ...);
std::string varfmtstring(char const* fmt, va_list list);

class RefCounted {
  uint32 ref_;
public:
  RefCounted() : ref_(1) {}
  virtual ~RefCounted() {}

  bool unique() const {
    return ref_ == 1;
  }
  uint32 addref();
  uint32 release();
};

void _qmemset(uint32* mem, uint32 fill, uint32 count);
uint32 gzdeflate(uint8 const* in, uint32 in_size, uint8* out, uint32* out_size);
uint32 gzencode(uint8 const* in, uint32 in_size, uint8* out, uint32* out_size);
uint32 gzinflate(uint8 const* in, uint32 in_size, uint8* out, uint32* out_size);
uint32 gzdecode(uint8 const* in, uint32 in_size, uint8* out, uint32* out_size);

template<class Ret, class... Args>
class Functor {
  class FunctionBase {
  public:
    virtual ~FunctionBase() {}
    virtual Ret run(Args... args) const = 0;
  };
  template<class F>
  class Function : public FunctionBase {
    F const& func_;
  public:
    Function(F const& f)
      : func_(f)
    {}
    Ret run(Args... args) const {
      return func_(args...);
    }
  };
  FunctionBase* func_;
public:
  template<class F>
  Functor(F const& f)
    : func_(new Function<F>(f))
  {}
  Functor(Functor const& f) = delete;
  ~Functor() {
    delete func_;
  }
  Functor(Functor&& f)
    : func_(f.func_)
  {
    f.func_ = nullptr;
  }
  Ret operator()(Args... args) const {
    return func_->run(args...);
  }
};

template<class... Args>
class FunctorNoRet {
  class FunctionBase {
  public:
    virtual ~FunctionBase() {}
    virtual void run(Args... args) const = 0;
  };
  template<class F>
  class Function : public FunctionBase {
    F const& func_;
  public:
    Function(F const& f)
      : func_(f)
    {}
    void run(Args... args) const {
      return func_(args...);
    }
  };
  FunctionBase* func_;
public:
  template<class F>
  FunctorNoRet(F const& f)
    : func_(new Function<F>(f))
  {}
  FunctorNoRet(FunctorNoRet const& f) = delete;
  ~FunctorNoRet() {
    delete func_;
  }
  FunctorNoRet(FunctorNoRet&& f)
    : func_(f.func_)
  {
    f.func_ = nullptr;
  }
  void operator()(Args... args) const {
    func_->run(args...);
  }
};

struct ci_char_traits : public std::char_traits < char > {
  static bool eq(char c1, char c2) { return std::toupper(c1) == std::toupper(c2); }
  static bool ne(char c1, char c2) { return std::toupper(c1) != std::toupper(c2); }
  static bool lt(char c1, char c2) { return std::toupper(c1) < std::toupper(c2); }
  static int compare(char const* s1, char const* s2, size_t n) {
    while (n--) {
      char c1 = std::toupper(*s1++);
      char c2 = std::toupper(*s2++);
      if (c1 != c2) return (c1 < c2 ? -1 : 1);
    }
    return 0;
  }
  static char const* find(char const* s, int n, char a) {
    a = std::toupper(a);
    while (n-- && std::toupper(*s) != a) {
      ++s;
    }
    return (n >= 0 ? s : nullptr);
  }
};
class istring : public std::basic_string<char, ci_char_traits> {
public:
  typedef std::basic_string<char, ci_char_traits> _Base;
  istring() {}
  istring(istring const& str) : _Base(str) {}
  istring(std::string const& str) : _Base(str.c_str()) {}
  istring(char const* str) : _Base(str) {}
  istring(istring&& str) : _Base(str) {}
  istring(char const* str, size_t n) : _Base(str) {}
  template<class Iter>
  istring(Iter begin, Iter end) : _Base(begin, end) {}

  istring& operator=(std::string const& str) {
    assign(str.c_str());
    return *this;
  }
  operator std::string() const {
    return std::string(c_str());
  }
};
template<class To>
using Map = std::map<istring, To>;
typedef Map<std::string> Dictionary;

std::string strlower(std::string const& src);

template<class T>
inline int basic_compare(T const& lhs, T const& rhs) {
  if (lhs < rhs) return -1;
  if (lhs > rhs) return 1;
  return 0;
}

std::vector<std::string> split(std::string const& str, char sep = ' ');
std::string join(std::vector<std::string> const& list, char sep = ' ');
std::string join(std::vector<std::string> const& list, std::string const& sep);
template<class Iter>
inline std::string join(Iter left, Iter right) {
  std::string res;
  while (left != right) {
    if (!res.empty()) res.push_back(' ');
    res.append(*left++);
  }
  return res;
}
