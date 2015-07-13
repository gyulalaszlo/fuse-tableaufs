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

    class Readdir {

      public:
      enum { Id = 0x01 };

      private:

      // Replace any invalid characters in filenames.
      // This method returns a copy of the input string.
      static std::string sanitize_filename(std::string filename)
      {
        std::replace(begin(filename), end(filename), '/', '_');
        return filename;
      }

      // Helper to output an error
      template <class Output>
        static void readdir_err( Output& o, ErrorCode err )
        {
          o.setErr( err );
          o.initEntries(0);
        }


      // Add all the directory entries to the buffer
      template <typename Output>
        static void add_directory_entries(Output& output, const PGQuery& query)
        {
          // Propagate any errors
          if (!query.ok()) {
            readdir_err( output, query.status() );
            return;
          }

          // reserve space for all files and the two dot specials
          const auto size = query.size();
          auto dirs = output.initEntries( size + 2);
          dirs.set(0, ".");
          dirs.set(1, "..");

          // add each subdirectory
          for (auto i = 0; i < size; ++i) {
            dirs.set(i + 2, sanitize_filename(query.get_string(i, TFS_WG_QUERY_NAME)));
          }

          output.setErr(NO_ERR);
        }



      public:


      template <class State, class Input, class Output>
        void operator()(State& state, const Input& input, Output& output) const
        {
          const auto path_res = parse_tableau_path( ((std::string)input.getPath()).c_str());

          if (path_res.failed()) {
            readdir_err(output, path_res.err );
            return;
          }

          const auto& path = path_res.value;

          // If we are asked to list a file, then we are straight out of luck
          if (path.level == PathNode::File) {
            readdir_err( output, -ENOTDIR );
            return;
          }

          switch (path.level) {
            case PathNode::Root:
              add_directory_entries(
                  output, PGQuery(state.connection.get(), TFS_WG_LIST_SITES));
              return;

            case PathNode::Site:
              add_directory_entries(
                  output, PGQuery(state.connection.get(), TFS_WG_LIST_PROJECTS,
                    std::array<const char*, 1>{{path.site.c_str()}}));
              return;

            case PathNode::Project:
              add_directory_entries(
                  output, PGQuery(state.connection.get(), TFS_WG_LIST_WORKBOOKS
                    " union all " TFS_WG_LIST_DATASOURCES,
                    std::array<const char*, 2>{
                    {path.site.c_str(), path.project.c_str()}}));
              return;

            default:
              fprintf(stderr, "Unknown node level found: %u\n", path.level);
              readdir_err( output, -EINVAL );
              return;
          }
        }
    };
  }
}
