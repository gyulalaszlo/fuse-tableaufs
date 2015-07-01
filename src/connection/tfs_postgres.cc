#include "tfs_postgres.hpp"

#include "libpq-fe.h"
#include "libpq/libpq-fs.h"

#include "pg_helpers.hpp"
#include "cpp14/make_unique.hpp"
#include "tfs_sql_fragments.h"

using namespace tableauFS;

namespace {

  class TFSPostgresImpl;


  // Builds a stat node for a path and fills the default (non-db-related)
  // fields.
  struct stat default_stat_for( const PathNode& path ) {
      struct stat node = { .st_blksize = BlockSize };

      // basic stat stuff: file type, nlinks, size of dirs
      if ( path.level < PathNode::File) {
        node.st_mode = S_IFDIR | 0555;   // read only
        node.st_nlink = 2;
        node.st_size = BlockSize;
        node.st_blocks = 1;
      } else if (path.level == PathNode::File) {
        node.st_mode = S_IFREG | 0444;   // read only
        node.st_nlink = 1;
      }
      return node;
  }



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
    }

    ~PgConnection() {
      // closes the connection if the connection is a valid one
      if (conn != nullptr) PQfinish(conn);
    }


    // A wrapper for PQexecParams()
    Result<PGresult*> run_query( const char* sql,
        int nParams,
        const char * const *paramValues)
    {
      // fail if no connection
      if (!ok()) return {-EINVAL, nullptr};

      fprintf(stderr, "Running SQL: '%s'\n", sql);

      auto res = PQexecParams(conn, 
          TFS_WG_LIST_SITES " and c.name = $1",
          nParams,
          nullptr,
          paramValues,
          // no binary params, so no param types, etc.
          nullptr, nullptr, 0);

      if (PQresultStatus(res) == PGRES_TUPLES_OK)
        return {NO_ERR, res};

      // Debug if we failed
      fprintf(stderr, "SQL Exec Failed: '%s' entries failed: %s/%s",
          sql,
          PQresultErrorMessage(res),
          PQerrorMessage(conn));
      // TODO: error handling
      return {-EINVAL, nullptr};
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
    }

    virtual ~TFSPostgresImpl()
    {
    }

    TFSPostgresImpl( const TFSPostgresImpl& other ) = delete;
    TFSPostgresImpl( TFSPostgresImpl&& other ) = default;

    private:

    // Tries to open a file and returns the file handle if successful
    virtual Result<FHandle> open_file(const PathNode& path, int mode) {
      const FHandle handle = 0;
      return { NO_ERR, handle};
    }

    // Tries to list a directory and returns the list of file names
    // if successful
    virtual Result<DirectoryList&> read_directory(DirectoryList& buffer) {
      return { NO_ERR, buffer };
    }

    // Tries to stat a file
    virtual Result<struct stat> get_attributes(const PathNode& path) {

      auto node = default_stat_for( path );
      //struct stat node = { .st_blksize = BlockSize };

      //// basic stat stuff: file type, nlinks, size of dirs
      //if ( path.level < PathNode::File) {
        //node.st_mode = S_IFDIR | 0555;   // read only
        //node.st_nlink = 2;
        //node.st_size = BlockSize;
        //node.st_blocks = 1;
      //} else if (path.level == PathNode::File) {
        //node.st_mode = S_IFREG | 0444;   // read only
        //node.st_nlink = 1;
      //}

      switch (path.level) {

        // Root nodes care only about a (fake) mtime
        case PathNode::Root:
          time( &(node.st_mtime));
          return {NO_ERR,node};

        case PathNode::Site:
          {
            const char* param_values[1] = { path.site.c_str() };
            const auto res = connection->run_query(
                TFS_WG_LIST_SITES " and c.name = $1",
                1, param_values
                );

            // Propagate any errors
            if (!res.status.ok()) return {res.status.err, node};

            // Fail if no such entry
            if (PQntuples(res.value) == 0 ) return {-ENOENT, node};

            // return shit:
            return {NO_ERR, node};
          }

        default:
          return {NO_ERR, node};
      }

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
    virtual Result<void> truncate_file(const PathNode& path, off_t offset) {
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
