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
