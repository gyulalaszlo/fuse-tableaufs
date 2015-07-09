#include "gtest/gtest.h"

#include <iostream>
#include <sstream>

#include "utils/kj/iostream.h"
#include "utils/debug_utilities.hpp"

#include "connection/tfs_capnp.hpp"

using namespace tableauFS;
using namespace kj::std;
using namespace kj;


void test_pathnode_roundtrip( const PathNode& n ) {
  ::std::stringstream ss;

  auto out = StdOutputStream(ss);
  auto in = StdInputStream(ss);
  kj::BufferedInputStreamWrapper in_buffered(in);

  encode_read_dir_req( out, n );

  const auto data = ss.str();
  monkeykingz::dump_binary( data.c_str(), data.size(), true );

  auto pn = PathNode{};

  decode_read_dir_req( in_buffered, pn );

  ASSERT_EQ( n.level   , pn.level );
  ASSERT_EQ( n.site    , pn.site );
  ASSERT_EQ( n.project , pn.project );
  ASSERT_EQ( n.file    , pn.file );
}

TEST(TFSCapnp, EncodeReaddir)
{
  test_pathnode_roundtrip(  PathNode{ PathNode::Root } );
  test_pathnode_roundtrip(  PathNode{ PathNode::Site, "sitename" } );
  test_pathnode_roundtrip(  PathNode{ PathNode::Project, "sitename", "projectname" } );
  test_pathnode_roundtrip(  PathNode{ PathNode::File, "sitename", "projectname", "file" } );
}

