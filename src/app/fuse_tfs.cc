
// fuse.h recommends setting the API version to 26 for new applications
#define FUSE_USE_VERSION 26

extern "C" {
#include <fuse.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "tableaufs_version.h"
}

#include <memory>
#include <utility>
#include "cpp14/make_unique.hpp"
#include "utils/cache.hpp"
#include "connection/parse_path.hpp"
#include "connection/tfs_postgres_config.hpp"
#include "connection/tfs_postgres.hpp"
#include "connection/pg_connection.hpp"

using namespace tableauFS;

namespace
{
  /** A structure with the Postgres connection parameters */
  struct tableau_cmdargs {
    const char* pghost;
    const char* pgport;
    const char* pguser;
    const char* pgpass;
  };

  // The global instance of the service provider used by fuse
  std::shared_ptr<TFSPostgres> tfs;

  // STAT
  // ----

  struct CachedStat {
    struct stat st;
    int result;
  };

  // The cache used for stating
  auto stat_cache = Cache<std::string, CachedStat>("stat");

  // Get the attributes
  int tableau_getattr(const char* path, struct stat* stbuf)
  {
    const auto path_str = std::string(path);
    auto stat = stat_cache.get(path_str, [&]() -> CachedStat {

      // try to parse the path
      const auto path_node = parse_tableau_path(path);
      if (!path_node.ok()) return {*stbuf, path_node.err};

      // get the stats
      auto stats = tfs->get_attributes(path_node.value);
      if (!stats.ok()) return {*stbuf, stats.err};

      return {stats.value, stats.err};
    });

    memcpy(stbuf, &stat.st, sizeof(struct stat));
    return stat.result;
  }

  // READDIR
  // -------

  struct CachedDirList {
    int result;
    DirectoryList entries;
  };

  // The cache used for stating
  auto dir_cache = Cache<std::string, CachedDirList>("readdir");

  // Readdir implementation
  int tableau_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info* fi)
  {
    const auto path_str = std::string(path);
    auto stat = dir_cache.get(path_str, [&]() -> CachedDirList {
      // prepare the cache
      auto dir_cache = DirectoryList{};

      // try to parse the path
      const auto path_node = parse_tableau_path(path);
      if (!path_node.ok()) return {path_node.err, dir_cache};

      // read the contents
      auto dir_list = tfs->read_directory(path_node.value, dir_cache);

      if (dir_list.failed()) {
        syslog(LOG_WARNING,
               "Cannot get the list of the directory '%s' ERROR:'%i'\n", path,
               dir_list.err);
      }

      return {dir_list.err, dir_list.value};

      //// get the stats
      // auto stats = tfs->get_attributes(path_node.value);
      // if (!stats.ok()) return {*stbuf, stats.err};

      // return {stats.value, stats.err};
    });

    if (stat.result != 0) return stat.result;

    for (const auto& e : stat.entries) {
      filler(buf, e.name.c_str(), NULL, 0);
    }

    return stat.result;

    ///

    // const auto path_node = parse_tableau_path(path);
    // if (!path_node.ok()) return path_node.err;

    // auto dir_cache = DirectoryList{};
    // auto dir_list = tfs->read_directory(path_node.value, dir_cache);

    // if (!dir_list.ok()) {
    // fprintf(stderr, "Cannot get the list of the directory '%s' ERROR:'%i'\n",
    // path, dir_list.err);
    // return dir_list.err;
    //}

    // for (const auto& e : dir_list.value) {
    // filler(buf, e.name.c_str(), NULL, 0);
    //}

    // return 0;
  }

  int tableau_open(const char* path, struct fuse_file_info* fi)
  {
    const auto path_node = parse_tableau_path(path);
    if (!path_node.ok()) return path_node.err;

    auto handle = tfs->open_file(path_node.value, fi->flags);

    if (!handle.ok()) return handle.err;
    fi->fh = handle.value;
    // during read we can return smaller buffer than
    // requested
    fi->direct_io = 1;
    return 0;
  }

  int tableau_read(const char* path, char* buf, size_t size, off_t offset,
                   struct fuse_file_info* fi)
  {
    auto res = tfs->read_file(fi->fh, monkeykingz::make_slice(buf, size), size,
                              offset);
    if (!res.ok()) return res.err;
    return res.value.size();
  }

#ifdef TFS_ALLOW_WRITE
  int tableau_write(const char* path, const char* buf, size_t size,
                    off_t offset, struct fuse_file_info* fi)
  {
    // return TFS_WG_IO_operation(TFS_WG_WRITE, fi->fh, buf, NULL, size,
    // offset);
    return 0;
  }

  int tableau_truncate(const char* path, off_t offset)
  {
    // tfs_wg_node_t node;
    int ret;

    // TFS_WG_PARSE_PATH(path, &node);
    // if (node.level != TFS_WG_FILE )
    // ret = -EISDIR;
    // else
    // ret = TFS_WG_IO_operation(TFS_WG_TRUNCATE, node.loid, NULL, NULL, 0,
    // offset);

    return ret;
  }
#endif

  // A descriptor for all the possible FUSE operations on a tableau endpoint
  struct fuse_operations tableau_oper = {
      .getattr = tableau_getattr,
      .readdir = tableau_readdir,
      .open = tableau_open,
      .read = tableau_read,
#ifdef TFS_ALLOW_WRITE
      .write = tableau_write,
      .truncate = tableau_truncate,
#endif
  };

// A shortcut macro for easy-peasy parameter description
#define TABLEAUFS_OPT(t, p)                   \
  {                                           \
    t, offsetof(struct tableau_cmdargs, p), 1 \
  }

  tableau_cmdargs tableau_cmdargs;
  struct fuse_opt tableaufs_opts[] = {
      TABLEAUFS_OPT("pghost=%s", pghost), TABLEAUFS_OPT("pgport=%s", pgport),
      TABLEAUFS_OPT("pguser=%s", pguser), TABLEAUFS_OPT("pgpass=%s", pgpass),

      // No more options for you Sir
      FUSE_OPT_END};

  // Just some verbose information.
  void print_verbose_information(int argc, char* argv[])
  {
    printf("TableauFS v%s using FUSE API Version %d\n", TABLEAUFS_VERSION,
           fuse_version());
    printf("  compiled with: %s\n", TFS_COMPILED_WITH);

    printf(
        "  features: "
#ifdef HAVE_LO_LSEEK64
        " lo_lseek64"
#endif
#ifdef HAVE_LO_TRUNCATE64
        " lo_truncate64"
#endif
#ifdef TFS_ALLOW_WRITE
        " allow_write"
#endif
        "\n");

    printf("  file_offset_bits=%d block_size=%d\n", _FILE_OFFSET_BITS,
           BlockSize);
  }

  std::shared_ptr<TFSPostgres> make_tfs_postgres(const Host host)
  {
    return TFSPostgres::make(std::make_unique<PgConnection>(host),
                             TFSPostgresConfig::with_dynamic_root_mtime());
  }
}

int main(int argc, char* argv[])
{
  // print some information
  print_verbose_information(argc, argv);

  // Parse the command line
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  if (fuse_opt_parse(&args, &tableau_cmdargs, tableaufs_opts, NULL) == -1)
    return -1;

  // Validate the options
  if (tableau_cmdargs.pguser == NULL || tableau_cmdargs.pghost == NULL ||
      tableau_cmdargs.pgport == NULL || tableau_cmdargs.pgpass == NULL) {
    fprintf(stderr,
            "Error: You should specify all of the following mount options:\n");
    fprintf(stderr, "\tpghost pgport pguser pgpass\n");
    return -1;
  }

  const auto host = Host{tableau_cmdargs.pghost, tableau_cmdargs.pgport,
                         tableau_cmdargs.pguser, tableau_cmdargs.pgpass};

  tfs = std::move(make_tfs_postgres(host));

  // Do a check for the root and list projects
  auto dir_cache = DirectoryList{};
  auto dir_list = tfs->read_directory({PathNode::Root}, dir_cache);

  if (!dir_list.ok()) {
    fprintf(
        stderr,
        "Cannot get the list of the root directory '%i'. Disconnecting...\n",
        dir_list.err);
    exit(dir_list.err);
  }

  for (const auto& e : dir_list.value) {
    fprintf(stderr, "  [] %s\n", e.name.c_str());
  }

  // struct TFS_PgActor* actor = TFS_makeActor();
  // TFS_destroyActor(actor);
  // Do the FUSE dance
  return fuse_main(args.argc, args.argv, &tableau_oper, NULL);
}
