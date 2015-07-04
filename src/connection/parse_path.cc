#include "parse_path.hpp"

#include <errno.h>
#include <cstdio>
#include <cstring>

namespace tableauFS
{
  // int parse_path(const char * path, tfs_wg_node_t * node);

  Result<PathNode> parse_tableau_path(const char* path)
  {
    // If this is the root node
    if (strlen(path) == 1 && path[0] == '/') return {NO_ERR, {PathNode::Root}};

    // TODO: As of FUSE 2.8, all path lengths are valid
    char buf[NameMax * 3] = {0};
    auto site = &buf[0 * NameMax];
    auto project = &buf[1 * NameMax];
    auto file = &buf[2 * NameMax];

#define _NAME_MAX "255"
    // use this to catch any extra names after the file name
    char extras[NameMax];
    const auto ret = sscanf(path, "/%" _NAME_MAX "[^/]/%" _NAME_MAX
                                  "[^/]/%" _NAME_MAX "[^/]%s",
                            site, project, file, extras);
#undef _NAME_MAX

    // Cannot parse? invalid!
    if (ret == EOF) {
      // TODO: returning errno is this so thread unsafe
      return {errno, {PathNode::Invalid}};
    }

    // check for invalid characters
    if (strchr(file, '/') != nullptr) {
      /* file name has / char in it */
      return {-EINVAL, {PathNode::Invalid}};
    }

    // fail if we havent consumed the whole string
    if (ret > 3) return {-EINVAL, {PathNode::Invalid}};

    // otherwise we should be fine
    // fprintf(stderr, "TFS_WG_parse_path: site: %s proj: %s file: %s\n",
    // site, project, file);

    return {NO_ERR, {(PathNode::Level)ret, site, project, file}};
  }
}
