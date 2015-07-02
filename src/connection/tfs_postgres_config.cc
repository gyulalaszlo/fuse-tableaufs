#include "tfs_postgres_config.hpp"

#include "cpp14/make_unique.hpp"

using namespace tableauFS;

namespace {
  // Class for setting the mtime of root to the current time
  class DynamicMtimeConfig : public TFSPostgresConfig {
    friend class TFSPostgresConfig;

    virtual time_t get_root_mtime() {
      time_t o;
      time( &o );
      return o;
    }
  };

  // Class for setting the mtime of root to the current time
  class StaticMtimeConfig : public TFSPostgresConfig {
    //friend class TFSPostgresConfig;
    friend class std::unique_ptr<StaticMtimeConfig>;

    public:
    StaticMtimeConfig( time_t default_time ) : o(default_time) { }

    virtual time_t get_root_mtime() { return o; }

    const time_t o;
  };


}


namespace tableauFS {

  std::unique_ptr<TFSPostgresConfig> TFSPostgresConfig::with_dynamic_root_mtime()
  {
    return std::make_unique<DynamicMtimeConfig>();
  }

  std::unique_ptr<TFSPostgresConfig> TFSPostgresConfig::with_static_root_mtime(time_t default_mtime)
  {
    return std::make_unique<StaticMtimeConfig>(default_mtime);
  }
}


