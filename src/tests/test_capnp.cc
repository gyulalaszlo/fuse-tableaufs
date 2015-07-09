#include "gtest/gtest.h"

#include <iostream>
#include <sstream>

#include "utils/kj/iostream.h"
#include "utils/debug_utilities.hpp"

#include "connection/tfs_capnp.hpp"

using namespace tableauFS;
using namespace kj::std;
using namespace kj;


// Test the roundtripping of a PathNode
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


// Test the roundtripping of a PathNode
void test_readdir_resp_roundtrip( const DirectoryList& l ) {
  ::std::stringstream ss;

  auto out = StdOutputStream(ss);
  auto in = StdInputStream(ss);
  kj::BufferedInputStreamWrapper in_buffered(in);

  encode_read_dir_resp( out, l );

  const auto data = ss.str();
  monkeykingz::dump_binary( data.c_str(), data.size(), true );

  auto lout = DirectoryList{};

  decode_read_dir_resp( in_buffered, lout );

  ASSERT_EQ( l.size(), lout.size() );
  for(int s=l.size(), i=0; i < s; ++i) {
    ASSERT_EQ( l[i].name, lout[i].name );
  }

}



TEST(TFSCapnp, EncodeReaddirReq)
{
  test_pathnode_roundtrip(  PathNode{ PathNode::Root } );
  test_pathnode_roundtrip(  PathNode{ PathNode::Site, "sitename" } );
  test_pathnode_roundtrip(  PathNode{ PathNode::Project, "sitename", "projectname" } );
  test_pathnode_roundtrip(  PathNode{ PathNode::File, "sitename", "projectname", "file" } );
}

TEST(TFSCapnp, EncodeReaddirResp)
{
  test_readdir_resp_roundtrip( DirectoryList{{ "."}, {".."}} );
  test_readdir_resp_roundtrip( DirectoryList{{ "."}, {".."}, {"Default"}} );
}

