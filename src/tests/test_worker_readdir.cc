#include "gtest/gtest.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <array>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "utils/kj/iostream.h"
#include "utils/debug_utilities.hpp"

#include "connection/tfs_capnp.hpp"

#include "connection/tfs_sql_fragments.h"
#include "connection/tfs_postgres_config.hpp"

#include "connection/parse_path.hpp"
#include "connection/pg_connection.hpp"
#include "connection/pg_query.hpp"

#include "workers/dispatcher.hpp"
#include "workers/worker_state.hpp"
#include "workers/worker_readdir.hpp"
#include "workers/worker_get_attributes.hpp"

#include "helper_test_pgconn.hpp"

using namespace tableauFS;
using namespace kj;
using namespace monkeykingz;

namespace
{
}

struct StreamPair {
  StreamPair()
      : ss_in()
      , ss_out()
      , out(ss_out)
      , in_unbuffered(ss_in)
      , from_output_unbuffered(ss_out)
      , to_input(ss_in)
      , in(in_unbuffered)
      , from_output(from_output_unbuffered)
  {
  }

  // Dump the hex contents of the stream
  void dump_out()
  {
    const auto data = ss_out.str();
    monkeykingz::dump_binary(data.c_str(), data.size(), true);
  }

  MKZ_NON_COPYABLE(StreamPair);

  std::stringstream ss_in;
  std::stringstream ss_out;

  kj::stdwrap::StdOutputStream out;
  kj::stdwrap::StdInputStream in_unbuffered;
  kj::stdwrap::StdInputStream from_output_unbuffered;

  kj::stdwrap::StdOutputStream to_input;

  kj::BufferedInputStreamWrapper in;
  kj::BufferedInputStreamWrapper from_output;
};

template <class Dispatcher>
void test_readdir(Dispatcher& dispatcher, const std::string& path,
                  const std::vector<std::string>& check_against)
{
  StreamPair s;
  {
    ::capnp::MallocMessageBuilder message;
    auto root = message.initRoot<proto::ReaddirReq>();
    root.setPath(path);
    writePackedMessage(s.to_input, message);
  }

  dispatcher.run(workers::Readdir::Id, s.in, s.out);

  {
    ::capnp::PackedMessageReader reader(s.from_output);

    auto root = reader.getRoot<proto::ReaddirResp>();
    ASSERT_EQ(NO_ERR, root.getErr());

    const auto& entries = root.getEntries();
    auto res = std::vector<std::string>(entries.size());

    using std::begin;
    using std::end;

    res.reserve(entries.size());
    for (const auto& e : entries) {
      res.emplace_back(e);
    }

    // std::copy(begin(entries), end(entries), begin(res) );
    std::sort(begin(res), end(res));

    // for (const auto& e : res) {
    // printf("    \"%s\",\n", e.c_str());
    //}

    // for (const auto& e : check_against ) {
    // printf("[%s]\n", e.c_str());
    //}

    ASSERT_EQ(check_against.size(), res.size());
    for (size_t i = 0; i < res.size(); ++i) {
      ASSERT_EQ(check_against[i], res[i]);
    }
  }
}

TEST(ReaddirWorker, Dispatch)
{
  auto dispatcher = Dispatcher<WorkerState>(
      std::make_unique<WorkerState>(make_pg_connection()));

  dispatcher.register_op<proto::ReaddirReq, proto::ReaddirResp>(
      workers::Readdir::Id, workers::Readdir{});
  dispatcher.register_op<proto::GetAttributesReq, proto::GetAttributesResp>(
      workers::GetAttributes::Id, workers::GetAttributes{});

  test_readdir(dispatcher, "/", {".", "..", "Default"});
  test_readdir(dispatcher, "/Default",
               {".", "..", "Tableau Samples", "default"});
  test_readdir(dispatcher, "/Default/default", {".", "..", "test.twb"});
  test_readdir(
      dispatcher, "/Default/Tableau Samples",
      {".", "..", "Dashboard Parameter Example Many Dashboards2.twbx",
       "Regional.twbx", "Superstore.twbx", "iphone5.twbx", "test.twb"});
}
