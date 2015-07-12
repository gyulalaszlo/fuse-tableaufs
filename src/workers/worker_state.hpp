#pragma once

#include <memory>

namespace tableauFS {

  struct WorkerState {

    WorkerState( std::unique_ptr<PgConnection> connection)
      : connection( std::move(connection) )
    {
    }

    std::unique_ptr<PgConnection> connection;
  };

}
