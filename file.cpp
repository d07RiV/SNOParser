#include "file.h"
#include <windows.h>
#include <set>

class StdFileBuffer : public FileBuffer {
  FILE* file_;
public:
  StdFileBuffer(FILE* file)
    : file_(file)
  {}
  ~StdFileBuffer() {
    fclose(file_);
  }

  int getc() {
    return fgetc(file_);
  }
  void putc(int chr) {
    fputc(chr, file_);
  }

  uint64 tell() const {
    return _ftelli64(file_);
  }
  void seek(int64 pos, int mode) {
    _fseeki64(file_, pos, mode);
  }

  size_t read(void* ptr, size_t size) {
    return fread(ptr, 1, size, file_);
  }
  size_t write(void const* ptr, size_t size) {
    return fwrite(ptr, 1, size, file_);
  }
};

File::File(char const* name, char const* mode)
  : file_(nullptr)
{
  FILE* file = fopen(name, mode);
  if (!file && (mode[0] == 'w' || mode[0] == 'a')) {
    std::string buf;
    for (int i = 0; name[i]; ++i) {
      char chr = (name[i] == '/' ? '\\' : name[i]);
      buf += chr;
      if (chr == '\\') {
        CreateDirectory(buf.c_str(), NULL);
      }
    }
    file = fopen(name, mode);
  }
  if (file) {
    file_ = new StdFileBuffer(file);
  }
}

void File::printf(char const* fmt, ...) {
  static char buf[1024];

  va_list ap;
  va_start(ap, fmt);

  int len = _vscprintf(fmt, ap);
  char* dst;
  if (len < 1024) {
    dst = buf;
  } else {
    dst = new char[len + 1];
  }
  vsprintf(dst, fmt, ap);
  file_->write(dst, len);
  if (dst != buf) {
    delete[] dst;
  }
}
bool File::getline(std::string& out) {
  out.clear();
  int chr;
  while ((chr = file_->getc()) != EOF) {
    if (chr == '\r') {
      char next = file_->getc();
      if (next != '\n') {
        file_->seek(-1, SEEK_CUR);
      }
      return true;
    }
    if (chr == '\n') {
      return true;
    }
    out.push_back(chr);
  }
  return !out.empty();
}
File::LineIterator File::begin() {
  return LineIterator(*this);
}
File::LineIterator File::end() {
  return LineIterator();
}

class MemFileBuffer : public FileBuffer {
  uint8 const* ptr_;
  uint8* clone_;
  size_t pos_;
  size_t size_;
public:
  MemFileBuffer(uint8 const* ptr, size_t size, bool clone)
    : ptr_(ptr)
    , pos_(0)
    , size_(size)
    , clone_(nullptr)
  {
    if (clone) {
      clone_ = new uint8[size];
      memcpy(clone_, ptr, size);
      ptr_ = clone_;
    }
  }
  ~MemFileBuffer() {
    delete[] clone_;
  }

  int getc() {
    return (pos_ < size_ ? ptr_[pos_++] : EOF);
  }
  void putc(int chr) {}

  uint64 tell() const {
    return pos_;
  }
  void seek(int64 pos, int mode) {
    switch (mode) {
    case SEEK_CUR:
      pos += pos_;
      break;
    case SEEK_END:
      pos += size_;
      break;
    }
    if (pos < 0) pos = 0;
    if (pos > size_) pos = size_;
    pos_ = pos;
  }
  uint64 size() {
    return size_;
  }

  size_t read(void* ptr, size_t size) {
    if (size + pos_ > size_) {
      size = size_ - pos_;
    }
    if (size) {
      memcpy(ptr, ptr_ + pos_, size);
      pos_ += size;
    }
    return size;
  }
  size_t write(void const* ptr, size_t size) {
    return 0;
  }
};

File File::memfile(void const* ptr, size_t size, bool clone) {
  return File(new MemFileBuffer((uint8*)ptr, size, clone));
}

class SubFileBuffer : public FileBuffer {
  File file_;
  uint64 start_;
  uint64 end_;
  uint64 pos_;
public:
  SubFileBuffer(File& file, uint64 offset, uint64 size)
    : file_(file)
    , start_(offset)
    , end_(offset + size)
    , pos_(offset)
  {
    file.seek(offset);
  }

  int getc() {
    if (pos_ >= end_) return EOF;
    ++pos_;
    return file_.getc();
  }
  void putc(int chr) {}

  uint64 tell() const {
    return pos_ - start_;
  }
  void seek(int64 pos, int mode) {
    switch (mode) {
    case SEEK_SET:
      pos += start_;
      break;
    case SEEK_CUR:
      pos += pos_;
      break;
    case SEEK_END:
      pos += end_;
      break;
    }
    if (pos < start_) pos = start_;
    if (pos > end_) pos = end_;
    pos_ = pos;
    file_.seek(pos_);
  }
  uint64 size() {
    return end_ - start_;
  }

  size_t read(void* ptr, size_t size) {
    if (size + pos_ > end_) {
      size = end_ - pos_;
    }
    if (size) {
      size = file_.read(ptr, size);
      pos_ += size;
    }
    return size;
  }
  size_t write(void const* ptr, size_t size) {
    return 0;
  }
};

File File::subfile(uint64 offset, uint64 size) {
  return File(new SubFileBuffer(*this, offset, size));
}

class MemoryBuffer : public FileBuffer {
  size_t pos_;
  uint8* data_;
  size_t size_;
  size_t alloc_;
  size_t grow_;
public:
  MemoryBuffer(size_t size, size_t grow)
    : alloc_(size)
    , grow_(grow)
    , size_(0)
    , pos_(0)
  {
    data_ = new uint8[size];
  }
  ~MemoryBuffer() {
    delete[] data_;
  }

  int getc() {
    if (pos_ < size_) return data_[pos_++];
    return EOF;
  }

  uint64 tell() const {
    return pos_;
  }
  void seek(int64 pos, int mode) {
    switch (mode) {
    case SEEK_CUR:
      pos += pos_;
      break;
    case SEEK_END:
      pos += size_;
      break;
    }
    if (pos < 0) pos = 0;
    if (pos > size_) pos = size_;
    pos_ = pos;
  }
  uint64 size() {
    return size_;
  }

  size_t read(void* ptr, size_t size) {
    if (size + pos_ > size_) {
      size = size_ - pos_;
    }
    if (size) {
      memcpy(ptr, data_ + pos_, size);
      pos_ += size;
    }
    return size;
  }

  size_t write(void const* ptr, size_t size) {
    memcpy(reserve(size), ptr, size);
    return size;
  }

  uint8 const* data() const {
    return data_;
  }
  uint8* reserve(uint32 size) {
    if (pos_ + size > alloc_) {
      while (alloc_ < pos_ + size) {
        if (alloc_ < grow_) alloc_ *= 2;
        else alloc_ += grow_;
      }
      uint8* temp = new uint8[alloc_];
      memcpy(temp, data_, size_);
      delete[] data_;
      data_ = temp;
    }
    uint8* res = data_ + pos_;
    pos_ += size;
    if (pos_ > size_) size_ = pos_;
    return res;
  }
  void resize(uint32 size) {
    size_ = size;
    if (pos_ > size) pos_ = size;
  }
};

MemoryFile::MemoryFile(size_t initial, size_t grow)
  : File(new MemoryBuffer(initial, grow))
{}
uint8 const* MemoryFile::data() const {
  MemoryBuffer* buffer = dynamic_cast<MemoryBuffer*>(file_);
  return (buffer ? buffer->data() : nullptr);
}
uint8* MemoryFile::reserve(uint32 size) {
  MemoryBuffer* buffer = dynamic_cast<MemoryBuffer*>(file_);
  return (buffer ? buffer->reserve(size) : nullptr);
}
void MemoryFile::resize(uint32 size) {
  MemoryBuffer* buffer = dynamic_cast<MemoryBuffer*>(file_);
  if (buffer) buffer->resize(size);
}
size_t MemoryFile::csize() const {
  MemoryBuffer* buffer = dynamic_cast<MemoryBuffer*>(file_);
  return (buffer ? buffer->size() : 0);
}

#include "zlib/zlib.h"

File& Archive::create(uint32 id) {
  auto& file = files_[id];
  file.resize(0);
  return file;
}
void Archive::write(File& file, bool compression) {
  std::map<uint32, std::vector<uint8>> data;
  for (auto& kv : files_) {
    auto& vec = data[kv.first];
    uint32 outSize = kv.second.size();
    vec.resize(outSize);
    if (compression) {
      gzencode(kv.second.data(), kv.second.size(), vec.data(), &outSize);
      vec.resize(outSize);
    } else {
      memcpy(vec.data(), kv.second.data(), outSize);
    }
  }
  uint32 count = data.size();
  file.write32(count);
  uint32 offset = count * 12 + 4;
  for (auto& kv : data) {
    file.write32(kv.first);
    file.write32(offset);
    file.write32(kv.second.size());
    offset += kv.second.size();
  }
  for (auto& kv : data) {
    file.write(kv.second.data(), kv.second.size());
  }
}
void Archive::load(File& file, bool compression) {
  if (file) {
    uint32 count = file.read32();
    for (uint32 i = 0; i < count; ++i) {
      file.seek(i * 12 + 4);
      uint32 id = file.read32();
      uint32 offset = file.read32();
      uint32 size = file.read32();
      file.seek(offset);
      MemoryFile& mem = files_[id];
      if (compression) {
        std::vector<uint8> temp(size);
        file.read(temp.data(), size);
        z_stream z;
        memset(&z, 0, sizeof z);
        z.next_in = temp.data();
        z.avail_in = size;
        z.total_in = size;
        z.next_out = nullptr;
        z.avail_out = 0;
        z.total_out = 0;

        int result = inflateInit2(&z, 16 + MAX_WBITS);
        if (result == Z_OK) {
          do {
            uint32 pos = mem.size();
            z.avail_out = size;
            z.next_out = mem.reserve(size);
            result = inflate(&z, Z_NO_FLUSH);
            mem.resize(pos + size - z.avail_out);
            if (result == Z_NEED_DICT || result == Z_DATA_ERROR || result == Z_MEM_ERROR) break;
          } while (result != Z_STREAM_END);
        }
      } else {
        file.read(mem.reserve(size), size);
      }
    }
  }
}
bool Archive::has(uint32 id) {
  return files_.count(id) != 0;
}

void Archive::compare(File& diff, Archive& lhs, Archive& rhs, char const*(*Func)(uint32)) {
  std::set<uint32> files;
  for (auto& kv : lhs.files_) files.insert(kv.first);
  for (auto& kv : rhs.files_) files.insert(kv.first);
  for (uint32 id : files) {
    uint32 lsize = (lhs.has(id) ? lhs.files_[id].size() : 0);
    uint32 rsize = (rhs.has(id) ? rhs.files_[id].size() : 0);
    char const* name = (Func ? Func(id) : nullptr);
    if (lsize != rsize) diff.printf("%-8u %-8u %-8u%s\n", id, lsize, rsize, name ? name : "");
  }
}
