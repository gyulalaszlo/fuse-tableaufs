
// fuse.h recommends setting the API version to 26 for new applications
#define FUSE_USE_VERSION 26


extern "C" {
#include <fuse.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "connection/pgconn_c.h"
#include "tableaufs_version.h"
}

#include <memory>
#include "cpp14/make_unique.hpp"
#include "connection/parse_path.hpp"
#include "connection/tfs_postgres_config.hpp"
#include "connection/tfs_postgres.hpp"
#include "connection/pg_connection.hpp"

using namespace tableauFS;

namespace {
  /** A structure with the Postgres connection parameters */
  struct tableau_cmdargs {
    const char *pghost;
    const char *pgport;
    const char *pguser;
    const char *pgpass;
  };

  std::shared_ptr<TFSPostgres> tfs;

  int tableau_getattr(const char *path, struct stat *stbuf)
  {
    //int res = 0;
    //tfs_wg_node_t node;

    //TFS_WG_PARSE_PATH(path, &node);

    //memcpy(stbuf, &(node.st), sizeof(struct stat));
    const auto path_node =  parse_tableau_path(path);
    if (!path_node.status.ok()) return path_node.status.err;

    auto stats = tfs->get_attributes(path_node.value);
    if ( !stats.status.ok() ) return stats.status.err;

    memcpy(stbuf, &stats.value, sizeof(struct stat));
    return stats.status.err;
  }

  int tableau_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
      off_t offset, struct fuse_file_info *fi)
  {
    //tfs_wg_node_t node;

    //TFS_WG_PARSE_PATH(path, &node);

    //if ( node.level == TFS_WG_FILE )
      //return -ENOTDIR;

    //filler(buf, ".", NULL, 0);
    //filler(buf, "..", NULL, 0);

    //TFS_WG_readdir(&node, buf, filler);
    const auto path_node =  parse_tableau_path(path);
    if (!path_node.status.ok()) return path_node.status.err;

    auto dir_cache = DirectoryList{};
    auto dir_list = tfs->read_directory( path_node.value, dir_cache);

    if (!dir_list.status.ok()) {
      fprintf(stderr, "Cannot get the list of the directory '%s' ERROR:'%i'\n", path, dir_list.status.err);
      return dir_list.status.err;
    }

    for (const auto& e : dir_list.value) {
      filler(buf, e.name.c_str(), NULL, 0 );
      fprintf(stderr, "  Adding file: [] %s\n", e.name.c_str());
    }

    return 0;
  }

  int tableau_open(const char *path, struct fuse_file_info *fi)
  {
    //tfs_wg_node_t node;
    int ret;
    const auto path_node =  parse_tableau_path(path);
    if (!path_node.status.ok()) return path_node.status.err;

    auto handle = tfs->open_file( path_node.value, fi->flags);

    if (!handle.status.ok()) return handle.status.err;
    fi->fh = handle.value;
    // during read we can return smaller buffer than
    // requested
    fi->direct_io = 1;

    //TFS_WG_PARSE_PATH(path, &node);

    //ret = TFS_WG_open(&node, fi->flags, &(fi->fh) );
    //fi->direct_io = 1; // during read we can return smaller buffer than
    //// requested

    return 0;
  }

  int tableau_read(const char *path, char *buf, size_t size, off_t offset,
      struct fuse_file_info *fi)
  {
    //return TFS_WG_IO_operation(TFS_WG_READ, fi->fh, NULL, buf, size, offset);
    auto buffer = std::vector<char>(size);
    auto res = tfs->read_file( fi->fh, monkeykingz::make_slice( buffer ), size, offset );

    if (!res.status.ok()) return res.status.err;


    
    return 0;
  }

  int tableau_write(const char *path, const char *buf, size_t size, off_t offset,
      struct fuse_file_info *fi)
  {
    //return TFS_WG_IO_operation(TFS_WG_WRITE, fi->fh, buf, NULL, size, offset);
    return 0;
  }

  int tableau_truncate(const char *path, off_t offset)
  {
    //tfs_wg_node_t node;
    int ret;

    //TFS_WG_PARSE_PATH(path, &node);
    //if (node.level != TFS_WG_FILE )
      //ret = -EISDIR;
    //else
      //ret = TFS_WG_IO_operation(TFS_WG_TRUNCATE, node.loid, NULL, NULL, 0, offset);

    return ret;
  }


  // A descriptor for all the possible FUSE operations on a tableau endpoint
  struct fuse_operations tableau_oper = {
    .getattr        = tableau_getattr,
    .readdir        = tableau_readdir,
    .open           = tableau_open,
    .read           = tableau_read,
    .write          = tableau_write,
    .truncate       = tableau_truncate,
  };


  // A shortcut macro for easy-peasy parameter description
#define TABLEAUFS_OPT(t, p) { t, offsetof(struct tableau_cmdargs, p), 1 }


  tableau_cmdargs tableau_cmdargs;
  struct fuse_opt tableaufs_opts[] =
  {
    TABLEAUFS_OPT("pghost=%s", pghost),
    TABLEAUFS_OPT("pgport=%s", pgport),
    TABLEAUFS_OPT("pguser=%s", pguser),
    TABLEAUFS_OPT("pgpass=%s", pgpass),

    // No more options for you Sir
    FUSE_OPT_END
  };

  // Just some verbose information.
  void print_verbose_information(int argc, char *argv[])
  {
    printf("TableauFS v%s using FUSE API Version %d\n", TABLEAUFS_VERSION, fuse_version());
  }



  std::shared_ptr<TFSPostgres> make_tfs_postgres(const Host host) {
    return TFSPostgres::make(
        std::make_unique<PgConnection>( host ),
        TFSPostgresConfig::with_dynamic_root_mtime()
        );
  }

}

int main(int argc, char *argv[])
{
  // print some information
  print_verbose_information(argc, argv);

  // Parse the command line
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  if (fuse_opt_parse(&args, &tableau_cmdargs, tableaufs_opts, NULL) == -1)
    return -1;

  // Validate the options
  if (tableau_cmdargs.pguser == NULL ||
      tableau_cmdargs.pghost == NULL ||
      tableau_cmdargs.pgport == NULL ||
      tableau_cmdargs.pgpass == NULL) {
    fprintf(stderr, "Error: You should specify all of the following mount options:\n");
    fprintf(stderr, "\tpghost pgport pguser pgpass\n");
    return -1;
  }

  // Connect to PG
  printf("Connecting to %s@%s:%s\n", tableau_cmdargs.pguser,
      tableau_cmdargs.pghost, tableau_cmdargs.pgport );

  const auto host = Host{ tableau_cmdargs.pghost, tableau_cmdargs.pgport, tableau_cmdargs.pguser, tableau_cmdargs.pgpass };


  tfs = std::move( make_tfs_postgres( host ) );

  // Do a check for the root and list projects
  auto dir_cache = DirectoryList{};
  auto dir_list = tfs->read_directory({PathNode::Root}, dir_cache);

  if (!dir_list.status.ok()) {
    fprintf(stderr, "Cannot get the list of the root directory '%i'. Disconnecting...\n", dir_list.status.err);
    exit(dir_list.status.err);
  }

  for (const auto& e : dir_list.value) {
    fprintf(stderr, "  [] %s\n", e.name.c_str());
  }
  //TFS_WG_connect_db( tableau_cmdargs.pghost, tableau_cmdargs.pgport,
      //tableau_cmdargs.pguser, tableau_cmdargs.pgpass);


  //struct TFS_PgActor* actor = TFS_makeActor();
  //TFS_destroyActor(actor);
  // Do the FUSE dance
  return fuse_main(args.argc, args.argv, &tableau_oper, NULL);
}
