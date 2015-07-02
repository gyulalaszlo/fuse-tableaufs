#pragma once

#include <memory>
#include "connection_types.hpp"

namespace tableauFS {

  // Interface class for configuration (dependency injection)
  class TFSPostgresConfig {
    MKZ_INTERFACE(TFSPostgresConfig);

    using Ptr = std::unique_ptr<TFSPostgresConfig>;
    // The function used to get the mtime of the root
    virtual time_t get_root_mtime() = 0;

    static Ptr with_dynamic_root_mtime();
    static Ptr with_static_root_mtime(time_t default_mtime);
  };

}

