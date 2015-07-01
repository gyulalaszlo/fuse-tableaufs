#include "tfs_postgres.hpp"

#include "libpq-fe.h"
#include "libpq/libpq-fs.h"

#include "pg_helpers.hpp"
#include "cpp14/make_unique.hpp"

using namespace tableauFS;

namespace {

  class TFSPostgresImpl;




  struct PgConnection {
    PgConnection(Host host)
      : host(host)
      , conn(nullptr)
    {
      // try to connect and store the connection
      auto conn_result = connect_to_pg( host );
      if (conn_result.status.ok()) {
        conn = conn_result.value;
      }
      printf("Connecting.. %zX\n", (uintptr_t)conn);
    }

    ~PgConnection() {
      // closes the connection if the connection is a valid one
      if (conn != nullptr) PQfinish(conn);

      printf("Disconnecting.. %lX\n", conn);
    }

    PgConnection( const PgConnection& ) = delete;
    PgConnection& operator=(const PgConnection& other) = delete;
    PgConnection( PgConnection&& ) = default;
    PgConnection& operator=(PgConnection&& other) = default;

    // is the connection established?
    bool ok() const { return conn != nullptr; }


    Host host;
    PGconn* conn;
  };

  class TFSPostgresImpl : public TFSPostgres
  {
    friend class TFSPostgres;

    public:
    TFSPostgresImpl( std::unique_ptr<PgConnection> connection )
      : connection(std::move(connection))
    {
      printf("TFSPostgresImpl\n");
    }

    virtual ~TFSPostgresImpl()
    {
      printf("~TFSPostgresImpl\n");
    }

    TFSPostgresImpl( const TFSPostgresImpl& other ) = delete;
    TFSPostgresImpl( TFSPostgresImpl&& other ) = default;

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


    //PgConnection connection;
    std::unique_ptr<PgConnection> connection;
  };
}


namespace tableauFS {

  std::shared_ptr<TFSPostgres> TFSPostgres::make( const Host host )
  {
    return std::make_shared<TFSPostgresImpl>( std::make_unique<PgConnection>(host) );
  }
}
