#pragma once

#include "types.h"
#include "json.h"
#include "snomap.h"
#include "parser.h"

template<class T> inline void _serialize(T& x, json::Visitor* visitor) { x.serialize(visitor); }
template<> inline void _serialize(unsigned int& x, json::Visitor* visitor) { visitor->onInteger(x); }
template<> inline void _serialize(int& x, json::Visitor* visitor) { visitor->onInteger(x); }
template<> inline void _serialize(char& x, json::Visitor* visitor) { visitor->onInteger(x); }
template<> inline void _serialize(uint32& x, json::Visitor* visitor) { visitor->onInteger(x); }
template<> inline void _serialize(sint32& x, json::Visitor* visitor) { visitor->onInteger(x); }
template<> inline void _serialize(uint16& x, json::Visitor* visitor) { visitor->onInteger(x); }
template<> inline void _serialize(sint16& x, json::Visitor* visitor) { visitor->onInteger(x); }
template<> inline void _serialize(uint8& x, json::Visitor* visitor) { visitor->onInteger(x); }
template<> inline void _serialize(sint8& x, json::Visitor* visitor) { visitor->onInteger(x); }
template<> inline void _serialize(sint64& x, json::Visitor* visitor) { visitor->onInteger(x); }
template<> inline void _serialize(uint64& x, json::Visitor* visitor) { visitor->onInteger(x); }
template<> inline void _serialize(float& x, json::Visitor* visitor) { visitor->onNumber(x); }
template<class T, int N>
inline void _serialize(T(&x)[N], json::Visitor* visitor) {
  visitor->onOpenArray();
  for (int i = 0; i < N; ++i) {
    _serialize(x[i], visitor);
  }
  visitor->onCloseArray();
}
template<int N>
inline void _serialize(char(&x)[N], json::Visitor* visitor) {
  visitor->onString(x);
}

struct TagValue {
  uint32 value;
  operator uint32() const {
    return value;
  }
  void serialize(json::Visitor* visitor) {
    uint8 exponent = (value >> 23) & 0xFF;
    if (exponent != 0 && exponent != 255) {
      visitor->onNumber(*(float*)&value);
    } else {
      visitor->onInteger(value);
    }
  }
};

template<class T>
struct Sno {
  uint32 id;
  operator uint32() const {
    return id;
  }
  std::string name() const {
    char const* name = T::name(id);
    return (name ? name : "");
  }
  void serialize(json::Visitor* visitor) {
    char const* name = T::name(id);
    if (name) {
      visitor->onIntegerEx(id, name);
    } else {
      visitor->onInteger(id);
    }
  }
};

struct GameBalanceId {
  uint32 id;
  operator uint32() const {
    return id;
  }
  std::string name() const {
    char const* name = SnoManager::gameBalance()[id];
    return (name ? name : "");
  }
  void serialize(json::Visitor* visitor) {
    char const* name = SnoManager::gameBalance()[id];
    if (name) {
      visitor->onIntegerEx(id, name);
    } else {
      visitor->onInteger(id);
    }
  }
};

template<class T>
class Serializable {
public:
  void serialize(json::Visitor* visitor) {
    visitor->onOpenMap();
    reinterpret_cast<T*>(this)->dump(visitor);
    visitor->onCloseMap();
  };
protected:
  template<class T>
  void _write(char const* name, T& x, json::Visitor* visitor) {
    visitor->onMapKey(name);
    _serialize(x, visitor);
  }
  void _write(char const* name, uint32 x, json::Visitor* visitor) {
    visitor->onMapKey(name);
    _serialize(x, visitor);
  }
};

#define declstruct(name) struct name : public Serializable<name>
#define dumpfunc() dump(json::Visitor* visitor)
#define expand(x) x
#define do_write(x) _write(#x, x, visitor)
#define do_write2(x,y) do{do_write(x);do_write(y);}while(0)
#define do_write3(x,y,z) do{do_write(x);do_write(y);do_write(z);}while(0)
#define do_write4(x,y,z,w) do{do_write(x);do_write(y);do_write(z);do_write(w);}while(0)
#define do_write5(x,y,z,w,t) do{do_write(x);do_write(y);do_write(z);do_write(w);do_write(t);}while(0)
#define do_write6(x,y,z,w,t,u) do{do_write(x);do_write(y);do_write(z);do_write(w);do_write(t);do_write(u);}while(0)
#define _get_write(_1,_2,_3,_4,_5,_6,name,...) name
#define dumpval(...) expand(_get_write(__VA_ARGS__,do_write6,do_write5,do_write4,do_write3,do_write2,do_write))expand((__VA_ARGS__))

struct SerializeData : public Serializable<SerializeData> {
  uint32 offset;
  uint32 size;
  void dumpfunc() {
    dumpval(offset, size);
  }
};

template<class T, size_t>
class ArrayDataImpl {
  ArrayDataImpl() = delete;
};
template<class T>
class ArrayDataImpl<T, 4> {
protected:
  ArrayDataImpl(SerializeData const& sd) {
    uint32 offset = sd.offset;
    uint32 size = sd.size;
    if (SnoParser::context->contains(offset, size)) {
      data_ = reinterpret_cast<T*>(SnoParser::context->data(offset));
      size_ = size / sizeof(T);
    } else {
      data_ = nullptr;
      size_ = 0;
    }
  }
  T* pdata() {
    return data_;
  }
public:
  T const* data() const {
    return data_;
  }
  uint32 size() const {
    return size_;
  }
private:
  T* data_;
  uint32 size_;
};
template<class T>
class ArrayDataImpl<T, 8> {
protected:
  ArrayDataImpl(SerializeData const& sd) {
    uint32 offset = sd.offset;
    uint32 size = sd.size;
    if (SnoParser::context->contains(offset, size)) {
      impl_ = new Impl;
      impl_->data = reinterpret_cast<T*>(SnoParser::context->data(offset));
      impl_->size = size / sizeof(T);
    } else {
      impl_ = nullptr;
    }
  }
  ~ArrayDataImpl() {
    delete impl_;
  }
  T* pdata() {
    return (impl_ ? impl_->data : nullptr);
  }
public:
  T const* data() const {
    return (impl_ ? impl_->data : nullptr);
  }
  uint32 size() const {
    return (impl_ ? impl_->size : 0);
  }
private:
  struct Impl {
    T* data;
    uint32 size;
  } *impl_;
};

template<class T>
using ArrayData = ArrayDataImpl<T, sizeof(T*)>;

template<class T>
class Array : public ArrayData<T> {
public:
  Array(SerializeData const& sd)
    : ArrayData<T>(sd)
  {
    for (uint32 i = 0; i < size(); ++i) {
      new(pdata() + i) T;
    }
  }
  ~Array() {
    for (uint32 i = 0; i < size(); ++i) {
      pdata()[i].~T();
    }
  }

  void serialize(json::Visitor* visitor) {
    visitor->onOpenArray();
    for (uint32 i = 0; i < size(); ++i) {
      _serialize(pdata()[i], visitor);
    }
    visitor->onCloseArray();
  };

  T const& operator[](uint32 index) const {
    return data()[index];
  }
  T& operator[](uint32 index) {
    return pdata()[index];
  }

  class ConstIterator {
    Array<T> const* array_;
    uint32 index_;
  public:
    ConstIterator(Array<T> const* array, uint32 index)
      : array_(array), index_(index)
    {}
    ConstIterator& operator++() {
      ++index_;
      return *this;
    }
    bool operator==(ConstIterator const& rhs) const {
      return index_ == rhs.index_;
    }
    bool operator!=(ConstIterator const& rhs) const {
      return index_ != rhs.index_;
    }
    T const& operator*() const {
      return (*array_)[index_];
    }
  };
  ConstIterator begin() const {
    return ConstIterator(this, 0);
  }
  ConstIterator end() const {
    return ConstIterator(this, size());
  }

  class Iterator {
    Array<T>* array_;
    uint32 index_;
  public:
    Iterator(Array<T>* array, uint32 index)
      : array_(array), index_(index)
    {}
    Iterator& operator++() {
      ++index_;
      return *this;
    }
    bool operator==(Iterator const& rhs) const {
      return index_ == rhs.index_;
    }
    bool operator!=(Iterator const& rhs) const {
      return index_ != rhs.index_;
    }
    T& operator*() const {
      return (*array_)[index_];
    }
  };
  Iterator begin() {
    return Iterator(this, 0);
  }
  Iterator end() {
    return Iterator(this, size());
  }
};

class Text : public ArrayData<char> {
public:
  Text(SerializeData const& sd)
    : ArrayData<char>(sd)
  {}
  void serialize(json::Visitor* visitor) {
    if (data()) {
      visitor->onString(data());
    } else {
      visitor->onNull();
    }
  }

  char const* text() const {
    return data();
  }
};

class Binary : public ArrayData<uint8> {
public:
  Binary()
    : ArrayData<uint8>(*reinterpret_cast<SerializeData*>(this))
  {}

  void serialize(json::Visitor* visitor) {
    if (data()) {
      visitor->onString(fmtstring("<binary:%u>", size()));
    } else {
      visitor->onNull();
    }
  }

  File file() const {
    return File::memfile(data(), size());
  }
};
