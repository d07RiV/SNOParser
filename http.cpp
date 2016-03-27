#include "http.h"
#include "common.h"
#pragma comment(lib, "winhttp.lib")

HttpRequest::HttpRequest(std::string const& url, RequestType type)
  : type_(type)
  , session_(nullptr)
  , connect_(nullptr)
  , request_(nullptr)
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
  std::wstring path(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);

  session_ = WinHttpOpen(L"ShrineTips", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
  if (!session_) return;
  connect_ = WinHttpConnect(session_, host.c_str(), urlComp.nPort, 0);
  if (!connect_) return;
  request_ = WinHttpOpenRequest(
    connect_,
    type == GET ? L"GET" : L"POST",
    path.c_str(),
    L"HTTP/1.1",
    WINHTTP_NO_REFERER,
    WINHTTP_DEFAULT_ACCEPT_TYPES,
    (urlComp.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0) | WINHTTP_FLAG_BYPASS_PROXY_CACHE);
}
HttpRequest::~HttpRequest() {
  if (request_) WinHttpCloseHandle(request_);
  if (connect_) WinHttpCloseHandle(connect_);
  if (session_) WinHttpCloseHandle(session_);
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
  if (!request_) return false;
  if (!WinHttpSendRequest(request_,
    headers_.empty() ? nullptr : headers_.c_str(), headers_.size(),
    post_.empty() ? nullptr : &post_[0], post_.size(), post_.size(), 0)) {
    return false;
  }
  return WinHttpReceiveResponse(request_, NULL);
}

uint32 HttpRequest::status() {
  if (!request_) return 0;

  DWORD statusCode = 0;
  uint32 size = sizeof statusCode;
  WinHttpQueryHeaders(request_, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX,
    &statusCode, &size, WINHTTP_NO_HEADER_INDEX);
  return statusCode;
}

File HttpRequest::response() {
  if (!request_) return File();

  DWORD size = 0, read;
  std::string buffer;
  do {
    if (!WinHttpQueryDataAvailable(request_, &size)) return File();
    if (!size) break;
    size_t pos = buffer.size();
    buffer.resize(pos + size);
    if (!WinHttpReadData(request_, &buffer[pos], size, &read)) return File();
  } while (size);
  return File::memfile(&buffer[0], buffer.size(), true);
}

File HttpRequest::get(std::string const& url) {
  HttpRequest request(url);
  if (!request.send() || request.status() != 200) return File();
  return request.response();
}

std::map<std::string, std::string> HttpRequest::headers() {
  std::map<std::string, std::string> result;
  if (!request_) return result;
  DWORD size;
  WinHttpQueryHeaders(request_, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX, nullptr, &size, WINHTTP_NO_HEADER_INDEX);
  if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return result;
  std::vector<wchar_t> raw((size + 1) / sizeof(wchar_t));
  WinHttpQueryHeaders(request_, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX, &raw[0], &size, WINHTTP_NO_HEADER_INDEX);
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
