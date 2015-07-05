#pragma once

#include "pg_connection.hpp"

namespace tableauFS
{
  // A RAII wrapper for a postgres query and results
  class PGQuery
  {
   public:
    // Run a query without parameters
    PGQuery(PgConnection* connection, const char* sql)
        : res(connection->run_query(sql, 0, nullptr))
    {
    }

    // Create a new query using the provided params
    template <typename Container>
    PGQuery(PgConnection* connection, const char* sql, const Container& params)
        : res(connection->run_query(sql, params))
    {
    }

    // clean up after ourselves
    ~PGQuery()
    {
      if (res.value != nullptr) {
        PQclear(res.value);
        printf("Destroying PGQuery\n");
      }
    }

    // is the result ok
    bool ok() const { return res.ok(); }
    bool failed() const { return res.failed(); }

    // The error code of running the query
    int status() const { return res.err; }

    // Get a string as the nTh column of the row_idx row
    const char* get_string(const int row_idx, const int col_idx) const
    {
      return PQgetvalue(res.value, row_idx, col_idx);
    }

    // Get an uint64_t from the nTh column of the row_idx row
    uint64_t get_uint64(const int row_idx, const int col_idx) const
    {
      return atoll(get_string(row_idx, col_idx));
    }

    // The number of rows returned by the query
    size_t size() const { return PQntuples(res.value); }

    // The result so errors can be forwarded
    Result<PGresult*> res;
  };
}
