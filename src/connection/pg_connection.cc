#include "pg_connection.hpp"

#include <errno.h>
#include "pg_helpers.hpp"

using namespace tableauFS;
namespace
{
  enum OpType { Read, Write, Truncate };

  Result<slice<char>> io_op_result(int ret_val, slice<char> buffer)
  {
    fprintf(stderr, "<- IO Result: %i -- %lu bytes\n", ret_val, buffer.size());
    if (ret_val < 0) {
      return {ret_val, buffer};
    }
    return {NO_ERR, monkeykingz::make_slice(buffer.data(), ret_val)};
  }

  Result<slice<char>> io_op(PgConnection* connection, OpType op, Loid loid,
                            slice<char> buffer, off_t offset, size_t size)
  {
    // LO operations only supported within transactions
    // On our FS one read is one transaction
    // While libpq is thread safe, still, we cannot have parallel
    // transactions from multiple threads on the same connection
    const auto transaction = connection->transaction();
    // Any large object descriptors that remain open at the end of a transaction
    // will be closed automatically.
    auto conn = connection->conn;
    auto fd = lo_open(conn, (Oid)loid, INV_READ);

    fprintf(stderr,
            "-> io_op(): doing [%i] from Loid:%llu fd:%i (l:%lu:o:%tu)\n", op,
            loid, fd, size, offset);

    if (
#ifdef HAVE_LO_LSEEK64
        lo_lseek64(conn, fd, offset, SEEK_SET)
#else
        lo_lseek(conn, fd, (int)offset, SEEK_SET)
#endif
        < 0) {
      return {-EINVAL, buffer};
    } else {
      switch (op) {
        case Read:
          return io_op_result(lo_read(conn, fd, buffer.data(), size), buffer);
          // return { lo_read(conn, fd, buffer.data(), size), buffer };
          break;
        case Write:
          return {lo_write(conn, fd, buffer.data(), size), buffer};
        case Truncate:
#ifdef HAVE_LO_TRUNCATE64
          return {lo_truncate64(conn, fd, offset), buffer};
#else
          return {lo_truncate(conn, fd, (size_t)offset), buffer};
#endif
        default:
          return {-EINVAL, buffer};
      }
    }
  }
}

namespace tableauFS
{
  // A wrapper for a postgres connection
  PgConnection::PgConnection(Host host) : host(host), conn(nullptr)
  {
    // try to connect and store the connection
    auto conn_result = connect_to_pg(host);
    if (conn_result.ok()) {
      conn = conn_result.value;
    }
  }

  PgConnection::~PgConnection()
  {
    // closes the connection if the connection is a valid one
    if (conn != nullptr) {
      PQfinish(conn);
      fprintf(stderr, "Disconnected from '%s:%s'...\n", host.host.c_str(),
              host.port.c_str());
    }
  }

  // A wrapper for PQexecParams()
  Result<PGresult*> PgConnection::run_query(const char* sql, int nParams,
                                            const char* const* paramValues)
  {
    // fail if no connection
    if (!ok()) return {-EINVAL, nullptr};

    fprintf(stderr, "Running SQL: '%s'\n", sql);
    for (uint32_t i = 0; i < nParams; ++i) {
      fprintf(stderr, "  param $%d : '%s'\n", i + 1, paramValues[i]);
    }

    auto res = PQexecParams(conn, sql, nParams, nullptr, paramValues,
                            // no binary params, so no param types, etc.
                            nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
      // Debug if we failed
      fprintf(stderr, "SQL Exec Failed: '%s' entries failed: %s/%s", sql,
              PQresultErrorMessage(res), PQerrorMessage(conn));
      // TODO: error handling
      return {-EINVAL, nullptr};
    }

    // Fail if no such entry
    if (PQntuples(res) == 0) return {-ENOENT, nullptr};

    // otherwise we should be OK
    return {NO_ERR, res};
  }

  void PgConnection::run_statement(const char* sql)
  {
    PQclear(PQexec(conn, sql));
  }

  Result<slice<char>> PgConnection::read_lo_block(Loid loid, slice<char> buffer,
                                                  size_t size, off_t offset)
  {
    return io_op(this, Read, loid, buffer, offset, size);
  }

  PgConnection::Transaction::Transaction(PgConnection* conn) : conn(conn)
  {
    if (conn->ok()) {
      conn->run_statement("BEGIN");
      fprintf(stderr, "========= Transaction START ===========\n");
    } else {
      fprintf(stderr, "Cannot start transaction: not connected\n");
    }
  }

  PgConnection::Transaction::~Transaction()
  {
    if (conn->ok()) {
      conn->run_statement("END");
      fprintf(stderr, "========= Transaction END ===========\n");
    } else {
      fprintf(stderr, "Cannot end transaction: not connected\n");
    }
  }
}
