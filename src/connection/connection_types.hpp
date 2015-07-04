#pragma once


#include <string>
#include <vector>
#include "slice.hpp"
#include "workgroup.h"

// include some default macros for EZ LIFE
#include "cpp14/markup.hpp"


// pre-declare some postgres stuff so we dont need to include the pg headers
// in other headers unless inline functions make it necessary
extern "C" {
  typedef struct pg_conn PGconn;
  typedef struct pg_result PGresult;
}

namespace tableauFS {

  enum {
    // The block size of the TableauFS. Larger block size results in
    // less blocks to transfer.
    BlockSize = 8196 * 4,

    BufSize = 1024,

    // The maximum size of the path components (site, project, file)
    NameMax = 255
  };

  // Alias the slice type here
  template <typename T> using slice = monkeykingz::slice<T>;

  // A file handle to make sense of it
  using FHandle = uint64_t;

  using ROBuffer = slice<const char>;
  using RWBuffer = slice<char>;

  // The large object id type used by postgres
  using Loid = uint64_t;

  // A directory entry returned by read_dir
  struct DirectoryEntry {
    std::string name;
    Loid loid;  // repo id, if file
  };

  // TODO: this is seriously bad...
  using DirectoryList = std::vector<DirectoryEntry>;

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


  // Pre-declare these
  class PGQuery;
  class PgConnection;
  class TFSPostgres;
  class TFSPostgresConfig;

}

