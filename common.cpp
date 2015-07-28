#include <stdarg.h>
#include <clocale>
#include <algorithm>
#include "common.h"

std::string fmtstring(char const* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  std::string res = varfmtstring(fmt, ap);
  va_end(ap);
  return res;
}
std::string varfmtstring(char const* fmt, va_list list) {
  uint32 len = _vscprintf(fmt, list);
  std::string dst;
  dst.resize(len + 1);
  vsprintf(&dst[0], fmt, list);
  dst.resize(len);
  return dst;
}

Exception::Exception(char const* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  uint32 len = _vscprintf(fmt, ap);
  std::string dst;
  dst.resize(len + 1);
  vsprintf(&dst[0], fmt, ap);
  dst.resize(len);

  buf_.str(dst);
}

uint32 RefCounted::addref() {
  return InterlockedIncrement(&ref_);
}
uint32 RefCounted::release() {
  if (!this) {
    return 0;
  }
  uint32 result = InterlockedDecrement(&ref_);
  if (!result) {
    delete this;
  }
  return result;
}

void _qmemset(uint32* mem, uint32 fill, uint32 count) {
  while (count--) {
    *mem++ = fill;
  }
}

#include "zlib/zlib.h"
#ifdef _WIN64
#pragma comment(lib, "zlib/zlib64.lib")
#else
#ifdef _DEBUG
#pragma comment(lib, "zlib/zlib.lib")
#else
#pragma comment(lib, "zlib/zlib_r.lib")
#endif
#endif

uint32 gzdeflate(uint8 const* in, uint32 in_size, uint8* out, uint32* out_size) {
  z_stream z;
  memset(&z, 0, sizeof z);
  z.next_in = const_cast<Bytef*>(in);
  z.avail_in = in_size;
  z.total_in = in_size;
  z.next_out = out;
  z.avail_out = *out_size;
  z.total_out = 0;

  memset(out, 0, *out_size);

  int result = deflateInit(&z, Z_DEFAULT_COMPRESSION);
  if (result == Z_OK) {
    result = deflate(&z, Z_FINISH);
    *out_size = z.total_out;
    deflateEnd(&z);
  }
  return (result == Z_STREAM_END ? 0 : -1);
}
uint32 gzencode(uint8 const* in, uint32 in_size, uint8* out, uint32* out_size) {
  z_stream z;
  memset(&z, 0, sizeof z);
  z.next_in = const_cast<Bytef*>(in);
  z.avail_in = in_size;
  z.total_in = in_size;
  z.next_out = out;
  z.avail_out = *out_size;
  z.total_out = 0;

  int result = deflateInit2(&z, 6, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
  if (result == Z_OK) {
    result = deflate(&z, Z_FINISH);
    *out_size = z.total_out;
    deflateEnd(&z);
  }
  return ((result == Z_OK || result == Z_STREAM_END) ? 0 : 1);
}
uint32 gzinflate(uint8 const* in, uint32 in_size, uint8* out, uint32* out_size) {
  z_stream z;
  memset(&z, 0, sizeof z);
  z.next_in = const_cast<Bytef*>(in);
  z.avail_in = in_size;
  z.total_in = in_size;
  z.next_out = out;
  z.avail_out = *out_size;
  z.total_out = 0;

  memset(out, 0, *out_size);

  int result = inflateInit(&z);
  if (result == Z_OK) {
    result = inflate(&z, Z_FINISH);
    *out_size = z.total_out;
    inflateEnd(&z);
  }
  return (z.avail_out == 0 ? 0 : -1);
}
uint32 gzdecode(uint8 const* in, uint32 in_size, uint8* out, uint32* out_size) {
  z_stream z;
  memset(&z, 0, sizeof z);
  z.next_in = const_cast<Bytef*>(in);
  z.avail_in = in_size;
  z.total_in = in_size;
  z.next_out = out;
  z.avail_out = *out_size;
  z.total_out = 0;

  int result = inflateInit2(&z, 16 + MAX_WBITS);
  if (result == Z_OK) {
    result = inflate(&z, Z_FINISH);
    *out_size = z.total_out;
    deflateEnd(&z);
  }
  return (z.avail_out == 0 ? 0 : 1);
}

std::string strlower(std::string const& str) {
  std::string dest(str.size(), ' ');
  std::transform(str.begin(), str.end(), dest.begin(), std::tolower);
  return dest;
}

std::vector<std::string> split(std::string const& str, char sep) {
  std::vector<std::string> res;
  std::string cur;
  for (char c : str) {
    if (c == sep) {
      res.push_back(cur);
      cur.clear();
    } else {
      cur.push_back(c);
    }
  }
  res.push_back(cur);
  return res;
}
std::string join(std::vector<std::string> const& list, char sep) {
  std::string res;
  for (auto& str : list) {
    if (!res.empty()) res.push_back(' ');
    res.append(str);
  }
  return res;
}
std::string join(std::vector<std::string> const& list, std::string const& sep) {
  std::string res;
  for (auto& str : list) {
    if (!res.empty()) res.append(sep);
    res.append(str);
  }
  return res;
}
