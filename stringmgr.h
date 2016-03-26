#pragma once

#include "types.h"
#include "file.h"

class StringManager {
  struct Buffer {
    uint32 start;
    uint32 size;
    uint8* data;
    Buffer* next;
    Buffer() : data(nullptr) {}
    ~Buffer() { delete[] data; }

    const uint8* get(uint32 offset);
  };

  File* file;
  Buffer* buffers;
  uint32 origin;
  uint32 size;
public:
  StringManager(File* file, uint32 origin)
    : file(file)
    , origin(origin)
    , buffers(nullptr)
    , size(file->size())
  {}

  const uint8* get(uint32 pos);
  const char* getstr(uint32 pos) {
    const uint8* ptr = get(pos);
    return (ptr ? (char*) ptr : "null");
  }

  void print(uint32 pos, FILE* file = stdin);
};

#include <winnt.h>
class ExeFile : public File {
public:
  ExeFile(std::string const& path);

  uint32 dataBase() const {
    return dataBase_;
  }
  uint32 dataPos() const{
    return dataPos_;
  }

  uint32 fromData(uint32 offset) const {
    return offset - dataBase_ + dataPos_;
  }
  uint32 toData(uint32 offset) const {
    return offset - dataPos_ + dataBase_;
  }

  char const* readString(uint32 offset) const {
    if (offset >= dataBase_ && offset < stringEnd_) {
      return (char*)(ptr_ + fromData(offset));
    } else {
      return nullptr;
    }
  }

private:
  uint8* ptr_;
  uint32 dataBase_;
  uint32 dataPos_;
  uint32 stringEnd_;
};
