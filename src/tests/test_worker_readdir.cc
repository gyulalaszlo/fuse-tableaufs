#include "gtest/gtest.h"

#include <iostream>
#include <sstream>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "utils/kj/iostream.h"
#include "utils/debug_utilities.hpp"

#include "connection/tfs_capnp.hpp"

#include "workers/worker_readdir.hpp"

using namespace tableauFS;
using namespace kj;

using Stat = struct stat;

struct StreamPair {

  StreamPair()
    : ss_in()
    , ss_out()
    , out(ss_out)
    , in_unbuffered(ss_in)
    , from_output_unbuffered(ss_out)
    , to_input(ss_in)
    , in(in_unbuffered)
    , from_output( from_output_unbuffered )
  {
  }


  // Dump the hex contents of the stream
  void dump_out() {
    const auto data = ss_out.str();
    monkeykingz::dump_binary( data.c_str(), data.size(), true );
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



template <typename MessageT>
void encode_path( kj::OutputStream& out, const std::string& path  ) {
  ::capnp::MallocMessageBuilder message;

  auto root = message.initRoot<MessageT>();
  root.setPath( path );

  writePackedMessage(out, message);
}



TEST(WorkerReaddir, Basic) {
  StreamPair s;
  auto worker = workers::make_readdir([](const std::string& path){
      return DirectoryList{ {"."}, {".."}, {"Default"}, {path.c_str()} };
      });
  encode_path<proto::ReaddirReq>( s.to_input, "/hello-world" );

  ASSERT_EQ( NO_ERR, worker->apply( s.in, s.out ) );

  {
    ::capnp::PackedMessageReader message(s.from_output);
    auto root = message.getRoot<proto::ReaddirResp>();
    ASSERT_EQ( 4, root.getEntries().size() );

    auto entries = root.getEntries();
    ASSERT_EQ( ".", (std::string) entries[0] );
    ASSERT_EQ( "..", (std::string) entries[1] );
    ASSERT_EQ( "Default", (std::string) entries[2] );
    ASSERT_EQ( "/hello-world", (std::string) entries[3] );
  }
}
