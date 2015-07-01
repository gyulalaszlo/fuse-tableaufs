#pragma once


#include <string>
#include <vector>
#include "slice.hpp"
#include "workgroup.h"

namespace tableauFS {

  // Alias the slice type here
  template <typename T>
  using slice = monkeykingz::slice<T>;

  // A file handle to make sense of it
  using FHandle = uint64_t;

  using ROBuffer = slice<const char>;
  using RWBuffer = slice<char>;

  //using Node = tfs_wg_node_t;


  // TODO: this is seriously bad...
  using DirectoryList = std::vector< std::string >;

  // A file node for path
  struct PathNode {
    enum Level { Root, Site, Project, File, Invalid };

    Level level;

    std::string site;
    std::string project;
    std::string file;
  };

  // Description of the host
  struct Host {
    std::string host;
    std::string port;
    std::string user;
    std::string password;
  };



}

