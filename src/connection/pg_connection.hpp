#pragma once

#include "connection_types.hpp"
#include "result.hpp"

namespace tableauFS {


  // A wrapper for a postgres connection
  class PgConnection {
    public:
      PgConnection(Host host);
      ~PgConnection();

      // A wrapper for PQexecParams()
      Result<PGresult*> run_query( const char* sql,
          int nParams,
          const char * const *paramValues);

      // A convinience wrapper for containers (arrays, vector and slices)
      template <typename Container>
      Result<PGresult*> run_query( const char* sql,
          const Container& params ) {
        return run_query( sql, params.size(), params.data() );
      }


      MKZ_NON_COPYABLE(PgConnection);
      MKZ_MOVABLE(PgConnection);

      // is the connection established?
      bool ok() const { return conn != nullptr; }


    private:
      Host host;
      PGconn* conn;
  };

}
