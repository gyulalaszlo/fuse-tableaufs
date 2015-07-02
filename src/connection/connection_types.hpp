#pragma once


#include <string>
#include <vector>
#include "slice.hpp"
#include "workgroup.h"

// include some default macros for EZ LIFE
#include "cpp14/markup.hpp"


// pre-declare some shit
extern "C" {
  typedef struct pg_conn PGconn;
  typedef struct pg_result PGresult;
}

namespace tableauFS {

  enum {
    // The block size of the TableauFS
    BlockSize = 8196,

    BufSize = 1024,

    // The maximum size of the path components (site, project, file)
    NameMax = 255
  };

  // Alias the slice type here
  template <typename T>
  using slice = monkeykingz::slice<T>;

  // A file handle to make sense of it
  using FHandle = uint64_t;

  using ROBuffer = slice<const char>;
  using RWBuffer = slice<char>;


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


  class PgConnection;
  class TFSPostgres;
  class TFSPostgresConfig;

}

