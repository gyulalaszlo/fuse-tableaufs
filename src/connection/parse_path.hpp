#pragma once

#include "connection_types.hpp"

namespace tableauFS
{
  // Parse a path into a tableau path node
  Result<PathNode> parse_tableau_path(const char* path);
}
