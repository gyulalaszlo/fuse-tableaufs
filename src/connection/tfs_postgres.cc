#include "tfs_postgres.hpp"

using namespace tableauFS;

namespace {

  class TFSPostgresImpl : public TFSPostgres
  {
    friend class TFSPostgres;

    public:
    TFSPostgresImpl( Host host )
      : host(host)
    {
    }

    ~TFSPostgresImpl() {}

    private:

    // Tries to open a file and returns the file handle if successful
    virtual Result<FHandle> open_file(const char* path, int mode) {
      const FHandle handle = 0;
      return { NO_ERR, handle};
    }

    // Tries to list a directory and returns the list of file names
    // if successful
    virtual Result<DirectoryList&> read_directory(DirectoryList& buffer) {
      return { NO_ERR, buffer };
    }

    // Tries to stat a file
    virtual Result<struct stat> get_attributes(const char* path) {
      return {NO_ERR, {}};
    }

    // Read the contents of a file into a buffer and returns the buffer itself
    virtual Result<RWBuffer> read_file(FHandle handle, RWBuffer buf, off_t offset) {
      return {NO_ERR, buf};
    }

    // Write the contents of the buffer to the file and return the success value
    virtual Result<void> write_file(FHandle handle, ROBuffer buf, off_t offset) {
      return {NO_ERR};
    }

    // Truncate...
    virtual Result<void> truncate_file(const char* path, off_t offset) {
      return {NO_ERR};
    }


    Host host;

  };
}


namespace tableauFS {

  std::shared_ptr<TFSPostgres> TFSPostgres::make_tfs_postgres( const Host host )
  {
    return std::make_shared<TFSPostgresImpl>( host );
  }

}
