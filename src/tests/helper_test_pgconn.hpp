#pragma once

#include "cpp14/make_unique.hpp"

#include "connection/tfs_postgres.hpp"
#include "connection/tfs_postgres_config.hpp"
#include "connection/pg_connection.hpp"

namespace tableauFS
{
  enum { DEFAULT_ROOT_MTIME = 1111 };
  const auto test_host = Host{TFS_TEST_SERVER, "8060", "readonly", "onlyread"};

  inline std::shared_ptr<TFSPostgres> make_tfs_postgres()
  {
    return TFSPostgres::make(
        std::make_unique<PgConnection>(test_host),
        TFSPostgresConfig::with_static_root_mtime(DEFAULT_ROOT_MTIME));
  }
}
