#pragma once


#include <string>
#include <vector>
#include "slice.hpp"

namespace tableauFS {

  // Alias the slice type here
  template <typename T>
  using slice = monkeykingz::slice<T>;

  // A file handle to make sense of it
  using FHandle = uint64_t;

  using ROBuffer = slice<const char>;
  using RWBuffer = slice<char>;


  // TODO: this is seriously bad...
  using DirectoryList = std::vector< std::string >;

  // Description of the host
  struct Host {
    std::string host;
    std::string port;
    std::string user;
    std::string password;
  };



}

