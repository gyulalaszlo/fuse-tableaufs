#include "tfs_postgres.hpp"

#include "libpq-fe.h"
#include "libpq/libpq-fs.h"

#include "cpp14/make_unique.hpp"
#include "cpp14/scope_exit.hpp"
#include "tfs_sql_fragments.h"

#include "tfs_postgres_config.hpp"

#include "pg_connection.hpp"

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


  class TFSPostgresImpl : public TFSPostgres
  {
    friend class TFSPostgres;

    public:
    TFSPostgresImpl(
        std::unique_ptr<PgConnection> connection,
        std::unique_ptr<TFSPostgresConfig> config
        )
      : connection(std::move(connection))
      , config( std::move(config))
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

      switch (path.level) {

        // Root nodes care only about a (fake) mtime
        case PathNode::Root:
          // get the mtime of root from the config
          node.st_mtime = config->get_root_mtime();
          return {NO_ERR,node};

        case PathNode::Site:
          {
            const auto res = connection->run_query(
                TFS_WG_LIST_SITES " and c.name = $1",
                std::array<const char*, 1>{ path.site.c_str()}
                );
            // clean up after ourselves
            SCOPE_EXIT(PQclear(res.value));

            // Propagate any errors
            if (!res.status.ok()) return {res.status.err, node};

            //// Fail if no such entry
            //if (PQntuples(res.value) == 0 ) return {-ENOENT, node};

            // set the of the node
            node.st_mtime = atoll( PQgetvalue(res.value, 0, TFS_WG_QUERY_MTIME) );
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
    std::unique_ptr<TFSPostgresConfig> config;
  };


}


namespace tableauFS {

  std::shared_ptr<TFSPostgres> TFSPostgres::make(
      std::unique_ptr<PgConnection> connection,
      std::unique_ptr<TFSPostgresConfig> config
      )
  {
    return std::make_shared<TFSPostgresImpl>( std::move(connection), std::move(config) );
  }

}
