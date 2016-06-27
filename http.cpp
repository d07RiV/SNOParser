#define NOMINMAX
#include "http.h"
#include "common.h"
#include <algorithm>
#pragma comment(lib, "winhttp.lib")

HttpRequest::SessionHolder::~SessionHolder() {
  if (request) WinHttpCloseHandle(request);
  if (connect) WinHttpCloseHandle(connect);
  if (session) WinHttpCloseHandle(session);
}

HttpRequest::HttpRequest(std::string const& url, RequestType type)
  : type_(type)
  , handles_(new SessionHolder)
{
  std::wstring url16 = utf8_to_utf16(url);
  URL_COMPONENTS urlComp;
  memset(&urlComp, 0, sizeof urlComp);
  urlComp.dwStructSize = sizeof urlComp;
  urlComp.dwSchemeLength = -1;
  urlComp.dwHostNameLength = -1;
  urlComp.dwUrlPathLength = -1;
  urlComp.dwExtraInfoLength = -1;

  if (!WinHttpCrackUrl(url16.c_str(), url.size(), 0, &urlComp)) {
    return;
  }

  std::wstring host(urlComp.lpszHostName, urlComp.dwHostNameLength);
  std::wstring path(urlComp.lpszUrlPath, urlComp.dwUrlPathLength + urlComp.dwExtraInfoLength);

  handles_->session = WinHttpOpen(L"SNOParser", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
  if (!handles_->session) return;
  handles_->connect = WinHttpConnect(handles_->session, host.c_str(), urlComp.nPort, 0);
  if (!handles_->connect) return;
  handles_->request = WinHttpOpenRequest(
    handles_->connect,
    type == GET ? L"GET" : L"POST",
    path.c_str(),
    L"HTTP/1.1",
    WINHTTP_NO_REFERER,
    WINHTTP_DEFAULT_ACCEPT_TYPES,
    (urlComp.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0) | WINHTTP_FLAG_BYPASS_PROXY_CACHE);
}

void HttpRequest::addHeader(std::string const& header) {
  headers_.append(utf8_to_utf16(header));
  headers_.append(L"\r\n");
}

void HttpRequest::addHeader(std::string const& name, std::string const& value) {
  addHeader(name + ": " + value);
}

static std::string urlencode(std::string const& str) {
  std::string dst;
  for (unsigned char c : str) {
    if (isalnum(c)) {
      dst.push_back(c);
    } else if (c == ' ') {
      dst.push_back('+');
    } else {
      char hex[4];
      sprintf_s(hex, sizeof hex, "%%%02X", c);
      dst.append(hex);
    }
  }
  return dst;
}

void HttpRequest::addData(std::string const& key, std::string const& value) {
  if (!post_.empty()) post_.push_back('&');
  post_.append(urlencode(key));
  post_.push_back('=');
  post_.append(urlencode(value));
}

bool HttpRequest::send() {
  if (!handles_->request) return false;
  if (!WinHttpSendRequest(handles_->request,
    headers_.empty() ? nullptr : headers_.c_str(), headers_.size(),
    post_.empty() ? nullptr : &post_[0], post_.size(), post_.size(), 0)) {
    return false;
  }
  return WinHttpReceiveResponse(handles_->request, NULL);
}

uint32 HttpRequest::status() {
  if (!handles_->request) return 0;

  DWORD statusCode = 0;
  uint32 size = sizeof statusCode;
  WinHttpQueryHeaders(handles_->request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX,
    &statusCode, &size, WINHTTP_NO_HEADER_INDEX);
  return statusCode;
}

class HttpBuffer : public FileBuffer {
  std::shared_ptr<HttpRequest::SessionHolder> handles_;
  size_t size_;
  uint8* data_;
  size_t pos_;
  size_t loaded_;
public:
  HttpBuffer(std::shared_ptr<HttpRequest::SessionHolder> const& handles, size_t size)
    : handles_(handles)
    , size_(size)
    , pos_(0)
    , loaded_(0)
  {
    data_ = new uint8[size];
  }
  ~HttpBuffer() {
    delete[] data_;
  }

  int getc() {
    if (pos_ < loaded_) {
      return data_[pos_++];
    } else if (pos_ >= size_) {
      return EOF;
    } else {
      uint8 chr;
      read(&chr, 1);
      return chr;
    }
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
    while (pos_ + size > loaded_) {
      DWORD nsize = 0, nread;
      if (!WinHttpQueryDataAvailable(handles_->request, &nsize) || !nsize) break;
      WinHttpReadData(handles_->request, data_ + loaded_, std::min<DWORD>(nsize, size_ - loaded_), &nread);
      loaded_ += nread;
    }
    if (size + pos_ > loaded_) {
      size = loaded_ - pos_;
    }
    if (size) {
      memcpy(ptr, data_ + pos_, size);
      pos_ += size;
    }
    return size;
  }

  size_t write(void const* ptr, size_t size) {
    return 0;
  }
};

File HttpRequest::response() {
  if (!handles_->request) return File();

  DWORD contentLength = 0;
  DWORD size = (sizeof contentLength);
  WinHttpQueryHeaders(handles_->request, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX,
    &contentLength, &size, WINHTTP_NO_HEADER_INDEX);

  return File(new HttpBuffer(handles_, contentLength));
}

File HttpRequest::get(std::string const& url) {
  HttpRequest request(url);
  if (!request.send() || request.status() != 200) return File();
  return request.response();
}

std::map<std::string, std::string> HttpRequest::headers() {
  std::map<std::string, std::string> result;
  if (!handles_->request) return result;
  DWORD size;
  WinHttpQueryHeaders(handles_->request, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX, nullptr, &size, WINHTTP_NO_HEADER_INDEX);
  if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return result;
  std::vector<wchar_t> raw((size + 1) / sizeof(wchar_t));
  WinHttpQueryHeaders(handles_->request, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX, &raw[0], &size, WINHTTP_NO_HEADER_INDEX);
  std::wstring cur;
  for (wchar_t wc : raw) {
    if (wc) {
      cur.push_back(wc);
    } else if (!cur.empty()) {
      std::string line = utf16_to_utf8(cur);
      cur.clear();

      size_t colon = line.find(':');
      if (colon == std::string::npos) continue;
      result.emplace(trim(line.substr(0, colon)), trim(line.substr(colon + 1)));
    }
  }
  return result;
}
