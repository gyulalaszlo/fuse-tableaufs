#pragma once

// eeek
#include <sys/stat.h>
#include <memory>


#include "connection_types.hpp"
#include "result.hpp"

namespace tableauFS {

  // A note for filenames:
  // ---------------------
  //
  // Almost all operations take a path which can be of any length.
  //
  // Changed in fuse 2.8.0 (regardless of API version) Previously, paths were
  // limited to a length of PATH_MAX.

  using file_open      = std::function< Result<FHandle>( const char* path, int mode ) >;
  using read_directory = std::function< Result<DirectoryList>() >;


  struct TFS {
    file_open open;
    read_directory read_dir;
  };

  // Interface for postgres operations.
  class TFSPostgres {
    public:


      // This is an interface...
      virtual ~TFSPostgres() {}

      // Tries to open a file and returns the file handle if successful
      virtual Result<FHandle> open_file(const char* path, int mode) = 0;

      // Tries to list a directory and returns the list of file names
      // if successful
      virtual Result<DirectoryList&> read_directory(DirectoryList& buffer) = 0;

      // Tries to stat a file
      virtual Result<struct stat> get_attributes(const char* path) = 0;

      // Read the contents of a file into a buffer and returns the buffer itself
      virtual Result<RWBuffer> read_file(FHandle handle, RWBuffer buf, off_t offset) = 0;

      // Write the contents of the buffer to the file and return the success value
      virtual Result<void> write_file(FHandle handle, ROBuffer buf, off_t offset) = 0;

      // Truncate...
      virtual Result<void> truncate_file(const char* path, off_t offset) = 0;


      static std::shared_ptr<TFSPostgres> make( const Host host );

  };

  // Constructor for the default implementation
}


