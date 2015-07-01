#include "pg_helpers.hpp"

#include "libpq-fe.h"
#include "libpq/libpq-fs.h"

#include <iostream>

namespace {
  const char* TFS_DATABASE = "workgroup";

}

namespace tableauFS {
  template <typename Stream>
  Stream& operator<<(Stream& s, const Host& host) {
    s
      << "Host { "
      << " host:" << host.host
      << " port:" << host.port
      << " user:" << host.user
      << " password:" << host.password
      << " }"
      ;
    return s;
  }

  Result<PGconn*> connect_to_pg( Host conn_data)
  {
    std::cerr << "Connecting to: " << conn_data << "\n";
    PGconn* new_conn = PQsetdbLogin(
        conn_data.host.c_str(),
        conn_data.port.c_str(),
        nullptr,
        nullptr,
        TFS_DATABASE,
        conn_data.user.c_str(),
        conn_data.password.c_str()
        );

    auto status = PQstatus(new_conn);
    /* check to see that the backend connection was successfully made */
    if (status == CONNECTION_BAD)
    {
      fprintf(stderr, "Connection to database '%s' failed.\n", conn_data.host.c_str());
      fprintf(stderr, "%s", PQerrorMessage(new_conn));
      PQfinish(new_conn);
      return { status, nullptr };
    }
    return {NO_ERR, new_conn};
  }
}
