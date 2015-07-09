#include "stringmgr.h"

const uint8* StringManager::Buffer::get(uint32 offset) {
  if (offset < start) return nullptr;
  uint32 end = offset - start;
  while (end < size && data[end]) {
    ++end;
  }
  return (end < size ? data + offset - start : nullptr);
}

const uint8* StringManager::get(uint32 pos) {
  if (pos < origin) return nullptr;
  if (pos >= size + origin) return nullptr;
  Buffer* best = nullptr;
  for (Buffer* cur = buffers; cur; cur = cur->next) {
    if (cur->start <= pos && cur->start + cur->size > pos) {
      if (!best || cur->start + cur->size > best->start + best->size) {
        best = cur;
      }
    }
  }
  const uint8* result = (best ? best->get(pos) : nullptr);
  if (result) return result;

  Buffer* buff = new Buffer;
  buff->start = pos;
  buff->size = 65536;
  buff->data = new uint8[buff->size];
  file->seek(buff->start - origin);
  buff->size = file->read(buff->data, buff->size);
  return buff->get(pos);
}

void StringManager::print(uint32 value, FILE* file) {
  const uint8* ptr = get(value);
  if (ptr) {
    for (uint32 pos = 0; ptr[pos]; ++pos) {
      if (ptr[pos] < 32 || ptr[pos] > 126) {
        ptr = nullptr;
        break;
      }
    }
  }
  if (ptr) {
    fprintf(file, "%-8d: %s\n", value, (char*) ptr);
  } else {
    fprintf(file, "%d\n", value);
  }
}
