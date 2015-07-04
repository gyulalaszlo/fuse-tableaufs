#pragma once

extern "C" {
#include <libpq-fe.h>
#include <libpq/libpq-fs.h>
}

#include "result.hpp"
#include "connection_types.hpp"

namespace tableauFS
{
  // Connect to a Postgres instance
  Result<PGconn*> connect_to_pg(Host conn_data);
}
