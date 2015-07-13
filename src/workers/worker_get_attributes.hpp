#pragma once

#include <errno.h>
#include <array>

#include <capnp/message.h>

#include "connection/tfs_sql_fragments.h"
#include "connection/tfs_postgres_config.hpp"

#include "connection/parse_path.hpp"
#include "connection/pg_connection.hpp"
#include "connection/pg_query.hpp"

#include "worker_types.hpp"

namespace tableauFS {
namespace workers {

  struct GetAttributes {
      enum { Id = 0x02 };
    // Helper to output an error
    template <class Output>
      static void err( Output& o, ErrorCode err )
      {
        o.setErr( err );
      }


    // Builds a stat node for a path and fills the default (non-db-related)
    // fields.
    static struct stat default_stat_for(const PathNode& path)
    {
      struct stat node = {};
      node.st_blksize = BlockSize;

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

    // Helper for common statting from a query.
    template <typename Pred>
     static Result<struct stat> update_stats(struct stat node, const PGQuery& query,
          Pred pred)
      {
        if (query.failed()) return {query.status(), node};
        // Set the mtime
        node.st_mtime = query.get_uint64(0, TFS_WG_QUERY_MTIME);
        // update with the predicate
        pred(node, query);
        return {NO_ERR, node};
      }

    template <class State, class Input, class Output>
      void operator()(State& state, const Input& input, Output& output) const
      {
        const auto path_res = parse_tableau_path( ((std::string)input.getPath()).c_str());

        if (path_res.failed()) {
          err(output, path_res.err );
          return;
        }

        const auto path = path_res.value;
        auto node = default_stat_for(path);
        auto& connection = state.connection;

        switch (path.level) {
          // Root nodes care only about a (fake) mtime
          case PathNode::Root:
            // get the mtime of root from the config
            //node.st_mtime = config->get_root_mtime();
            time(&node.st_mtime);
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
            root.setErr(NO_ERR);
            return;
        }

      }
  };
}}
