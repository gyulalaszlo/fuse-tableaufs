#include "pg_connection.hpp"

#include "pg_helpers.hpp"

namespace tableauFS {

  // A wrapper for a postgres connection
  PgConnection::PgConnection(Host host)
    : host(host)
      , conn(nullptr)
  {
    // try to connect and store the connection
    auto conn_result = connect_to_pg( host );
    if (conn_result.status.ok()) {
      conn = conn_result.value;
    }
  }

  PgConnection::~PgConnection() {
    // closes the connection if the connection is a valid one
    if (conn != nullptr) {
      PQfinish(conn);
      fprintf( stderr, "Disconnected from '%s:%s'...\n", host.host.c_str(), host.port.c_str());
    }
  }

  // A wrapper for PQexecParams()
  Result<PGresult*> PgConnection::run_query( const char* sql,
      int nParams,
      const char * const *paramValues)
  {
    // fail if no connection
    if (!ok()) return {-EINVAL, nullptr};

    fprintf(stderr, "Running SQL: '%s'\n", sql);
    for(uint32_t i=0; i < nParams; ++i) {
      fprintf(stderr, "  param $%d : '%s'\n", i+1, paramValues[i]);
    }

    auto res = PQexecParams(conn,
        sql,
        nParams,
        nullptr,
        paramValues,
        // no binary params, so no param types, etc.
        nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
      // Debug if we failed
      fprintf(stderr, "SQL Exec Failed: '%s' entries failed: %s/%s",
          sql,
          PQresultErrorMessage(res),
          PQerrorMessage(conn));
      // TODO: error handling
      return {-EINVAL, nullptr};
    }

    // Fail if no such entry
    if (PQntuples(res) == 0 ) return {-ENOENT, nullptr};

    // otherwise we should be OK
    return {NO_ERR, res};
  }

}
