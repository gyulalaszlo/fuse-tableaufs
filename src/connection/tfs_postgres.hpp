#pragma once

// eeek
#include <sys/stat.h>
#include <memory>


#include "connection_types.hpp"
#include "result.hpp"

namespace tableauFS {

  // Interface for postgres operations.
  class TFSPostgres {
    MKZ_INTERFACE(TFSPostgres);

    // Tries to open a file and returns the file handle if successful
    virtual Result<FHandle> open_file(const PathNode& path, int mode) = 0;

    // Tries to list a directory and returns the list of file names
    // if successful
    virtual Result<DirectoryList&> read_directory(const PathNode& path, DirectoryList& buffer) = 0;

    // Tries to stat a file
    virtual Result<struct stat> get_attributes(const PathNode& path) = 0;

    // Read the contents of a file into a buffer and returns the buffer itself
    virtual Result<RWBuffer> read_file(FHandle handle, RWBuffer buf, off_t offset) = 0;

    // Write the contents of the buffer to the file and return the success value
    virtual Result<void> write_file(FHandle handle, ROBuffer buf, off_t offset) = 0;

    // Truncate...
    virtual Result<void> truncate_file(const PathNode& path, off_t offset) = 0;


    static std::shared_ptr<TFSPostgres> make(
        std::unique_ptr<PgConnection> connection,
        std::unique_ptr<TFSPostgresConfig> config
        );

  };

  // Constructor for the default implementation
}


