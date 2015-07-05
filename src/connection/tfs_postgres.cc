#include "tfs_postgres.hpp"

#include <libpq-fe.h>
#include <libpq/libpq-fs.h>
#include <errno.h>

#include <array>
#include <algorithm>

#include "cpp14/make_unique.hpp"
#include "cpp14/scope_exit.hpp"

#include "tfs_sql_fragments.h"
#include "tfs_postgres_config.hpp"

#include "pg_connection.hpp"
#include "pg_query.hpp"

using namespace tableauFS;

namespace
{
  class TFSPostgresImpl;

  // The indices in the SQL queries
  enum {
    TFS_WG_QUERY_NAME = 0,
    TFS_WG_QUERY_MTIME = 1,
    TFS_WG_QUERY_CONTENT = 2,
    TFS_WG_QUERY_SIZE = 3
  };

  // Builds a stat node for a path and fills the default (non-db-related)
  // fields.
  struct stat default_stat_for(const PathNode& path)
  {
    struct stat node = {.st_blksize = BlockSize};

    // basic stat stuff: file type, nlinks, size of dirs
    if (path.level < PathNode::File) {
      node.st_mode = S_IFDIR | 0555;  // read only
      node.st_nlink = 2;
      node.st_size = BlockSize;
      node.st_blocks = 1;
    } else if (path.level == PathNode::File) {
      node.st_mode = S_IFREG | 0444;  // read only
      node.st_nlink = 1;
    }
    return node;
  }

  // Replace any invalid characters in filenames
  //
  // Instead of references we try to use move semantics to
  // make this memory effective.
  std::string sanitize_filename(std::string filename)
  {
    std::replace(begin(filename), end(filename), '/', '_');
    // return the reference so we can chain these calls
    return filename;
  }

  // Add all the directory entries to the buffer
  template <typename Buffer>
  Result<Buffer&> add_directory_entries(Buffer& buffer, const PGQuery& query)
  {
    using Entry = typename Buffer::value_type;

    // clear the buffer
    buffer.clear();

    // Propagate any errors
    if (!query.ok()) return {query.status(), buffer};

    // reserve space for all files and the two dot specials
    const auto size = query.size();
    buffer.reserve(size + 2);

    // Add the dot and other stuff
    buffer.emplace_back(Entry{"."});
    buffer.emplace_back(Entry{".."});

    // add each subdirectory
    for (auto i = 0; i < size; ++i) {
      buffer.emplace_back(
          Entry{sanitize_filename(query.get_string(i, TFS_WG_QUERY_NAME))});
    }

    // Sort the directory list by alphabetic order so we can test the code
    // reliably.
    // TODO: this sort may be better of by being case-insensitive
    std::sort(begin(buffer), end(buffer),
              [&](const Entry& a, const Entry& b) { return a.name < b.name; });

    // return the done buffer
    return {NO_ERR, buffer};
  }

  // Helper for common statting from a query.
  template <typename Pred>
  Result<struct stat> update_stats(struct stat node, const PGQuery& query,
                                   Pred pred)
  {
    if (query.failed()) return {query.status(), node};
    // Set the mtime
    node.st_mtime = query.get_uint64(0, TFS_WG_QUERY_MTIME);
    // update with the predicate
    pred(node, query);
    return {NO_ERR, node};
  }

  class TFSPostgresImpl : public TFSPostgres
  {
    friend class TFSPostgres;

   public:
    TFSPostgresImpl(std::unique_ptr<PgConnection> connection,
                    std::unique_ptr<TFSPostgresConfig> config)
        : connection(std::move(connection)), config(std::move(config))
    {
    }

    virtual ~TFSPostgresImpl() {}

    TFSPostgresImpl(const TFSPostgresImpl& other) = delete;
    TFSPostgresImpl(TFSPostgresImpl&& other) = default;

   private:
    // Tries to open a file and returns the file handle if successful
    virtual Result<FHandle> open_file(const PathNode& path, int mode)
    {
      if (path.level != PathNode::File) return {-EISDIR, 0};

      const auto q = PGQuery( connection.get(),
          TFS_WG_LIST_WORKBOOKS " and $3 IN (" TFS_WG_NAMES_WITHOUT_SLASH(twb) ") "
          "union all "
          TFS_WG_LIST_DATASOURCES " and $3 IN (" TFS_WG_NAMES_WITHOUT_SLASH(tds) ") ",
          std::array<const char*, 3>{{ path.site.c_str(), path.project.c_str(), path.file.c_str() }}
          );

      // Propagate any errors
      if (!q.ok()) return {q.status(), 0};

      return {NO_ERR, q.get_uint64(0, TFS_WG_QUERY_CONTENT)};
    }

    // Tries to list a directory and returns the list of file names
    // if successful
    virtual Result<DirectoryList&> read_directory(const PathNode& path,
                                                  DirectoryList& buffer)
    {
      // If we are asked to list a file, then we are straight out of luck
      if (path.level == PathNode::File) return {-ENOTDIR, buffer};

      switch (path.level) {
        case PathNode::Root:
          return add_directory_entries(
              buffer, PGQuery(connection.get(), TFS_WG_LIST_SITES));

        case PathNode::Site:
          return add_directory_entries(
              buffer, PGQuery(connection.get(), TFS_WG_LIST_PROJECTS,
                              std::array<const char*, 1>{{path.site.c_str()}}));

        case PathNode::Project:
          return add_directory_entries(
              buffer, PGQuery(connection.get(), TFS_WG_LIST_WORKBOOKS
                              " union all " TFS_WG_LIST_DATASOURCES,
                              std::array<const char*, 2>{
                                  {path.site.c_str(), path.project.c_str()}}));

        default:
          fprintf(stderr, "Unknown node level found: %u\n", path.level);
          return {-EINVAL, buffer};
      }
    }

    // Tries to stat a file
    virtual Result<struct stat> get_attributes(const PathNode& path)
    {
      auto node = default_stat_for(path);

      switch (path.level) {
        // Root nodes care only about a (fake) mtime
        case PathNode::Root:
          // get the mtime of root from the config
          node.st_mtime = config->get_root_mtime();
          break;

        case PathNode::Site:

          return update_stats(
              node,
              PGQuery(connection.get(), TFS_WG_LIST_SITES " and c.name = $1",
                      std::array<const char*, 1>{{path.site.c_str()}}),
              [](struct stat& node, const PGQuery& q) {});

        case PathNode::Project:
          return update_stats(
              node,
              PGQuery(connection.get(), TFS_WG_LIST_PROJECTS " and c.name = $2",
                      std::array<const char*, 2>{
                          {path.site.c_str(), path.project.c_str()}}),
              [](struct stat& node, const PGQuery& q) {});

        case PathNode::File:
          return update_stats( node,
              PGQuery(
                connection.get(),
                TFS_WG_LIST_WORKBOOKS " and $3 IN (" TFS_WG_NAMES_WITHOUT_SLASH(twb) ") "
                "union all "
                TFS_WG_LIST_DATASOURCES " and $3 IN (" TFS_WG_NAMES_WITHOUT_SLASH(tds) ") ",
                std::array<const char*, 3>{{ path.site.c_str(), path.project.c_str(), path.file.c_str() }}
                ),
              [](struct stat& node, const PGQuery& query) {
                // set the size and block count
                const auto st_size = query.get_uint64(0, TFS_WG_QUERY_SIZE );

                node.st_size = st_size;
                if (st_size > 0) node.st_blocks = (int)st_size / BlockSize + 1;
              }
              );

        default:
          return {NO_ERR, node};
      }

      return {NO_ERR, node};
    }

    // Read the contents of a file into a buffer and returns the buffer itself
    virtual Result<RWBuffer> read_file(FHandle handle, RWBuffer buf,
                                       size_t size, off_t offset)
    {
      return connection->read_lo_block(handle, buf, size, offset);
    }

    // Write the contents of the buffer to the file and return the success value
    virtual Result<void> write_file(FHandle handle, ROBuffer buf, off_t offset)
    {
      return {NO_ERR};
    }

    // Truncate...
    virtual Result<void> truncate_file(const PathNode& path, off_t offset)
    {
      return {NO_ERR};
    }

    // PgConnection connection;
    std::unique_ptr<PgConnection> connection;
    std::unique_ptr<TFSPostgresConfig> config;
  };
}

namespace tableauFS
{
  std::shared_ptr<TFSPostgres> TFSPostgres::make(
      std::unique_ptr<PgConnection> connection,
      std::unique_ptr<TFSPostgresConfig> config)
  {
    return std::make_shared<TFSPostgresImpl>(std::move(connection),
                                             std::move(config));
  }
}
