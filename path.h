#pragma once

#include <string>

namespace path {
  std::string name(std::string const& path);
  std::string title(std::string const& path);
  std::string path(std::string const& path);
  std::string ext(std::string const& path);

  std::string const& root();
  std::string const& work();
  std::string const& casc();
  static const char sep = '\\';
}

std::string operator / (std::string const& lhs, std::string const& rhs);
