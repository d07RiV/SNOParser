#pragma once

#include <windows.h>
#include <winhttp.h>
#include <memory>
#include <string>
#include "file.h"

class HttpRequest {
public:
  enum RequestType {GET, POST};

  HttpRequest(std::string const& url, RequestType type = GET);

  void addHeader(std::string const& name, std::string const& value);
  void addHeader(std::string const& header);
  void addData(std::string const& key, std::string const& value);

  bool send();
  uint32 status();
  std::map<std::string, std::string> headers();
  File response();

  static File get(std::string const& url);

private:
  struct SessionHolder {
    HINTERNET session = nullptr;
    HINTERNET connect = nullptr;
    HINTERNET request = nullptr;
    ~SessionHolder();
  };
  friend class HttpBuffer;
  std::shared_ptr<SessionHolder> handles_;
  RequestType type_;
  std::wstring headers_;
  std::string post_;
};
