#include "gtest/gtest.h"

#include <iostream>
#include <sstream>

#include "utils/kj/iostream.h"
#include "utils/debug_utilities.hpp"

#include "connection/tfs_capnp.hpp"

using namespace tableauFS;
using namespace kj;

using Stat = struct stat;

struct RoundtripStreams {

  RoundtripStreams()
    : ss()
    , out(ss)
    , in(ss)
    , in_buffered(in)
  {
  }


  // Dump the hex contents of the stream
  void dump() {
    const auto data = ss.str();
    monkeykingz::dump_binary( data.c_str(), data.size(), true );
  }

  MKZ_NON_COPYABLE(RoundtripStreams);

  std::stringstream ss;
  kj::stdwrap::StdOutputStream out;
  kj::stdwrap::StdInputStream in;
  kj::BufferedInputStreamWrapper in_buffered;
};


// Test the roundtripping of a PathNode
void test_readdir_req_roundtrip( const std::string& n ) {
  RoundtripStreams o;
  auto path_back = std::string{};

  encode_read_dir_req( o.out, n.c_str() );
  decode_read_dir_req( o.in_buffered, path_back );

  ASSERT_EQ( n, path_back );
}


// Test the roundtripping of a PathNode
void test_readdir_resp_roundtrip( const DirectoryList& l ) {
  RoundtripStreams o;
  auto lout = DirectoryList{};

  encode_read_dir_resp( o.out, l );
  decode_read_dir_resp( o.in_buffered, lout );

  ASSERT_EQ( l.size(), lout.size() );
  for(int s=l.size(), i=0; i < s; ++i) {
    ASSERT_EQ( l[i].name, lout[i].name );
  }

}

// Test the roundtripping of a PathNode
void test_getattrs_resp_roundtrip( const struct stat& st ) {
  RoundtripStreams o;
  auto st_out = Stat{};

  encode_get_attributes_resp( o.out, st );
  decode_get_attributes_resp( o.in_buffered, st_out );

   ASSERT_EQ( st.st_mtime, st_out.st_mtime );
   ASSERT_EQ( st.st_mode, st_out.st_mode );
   ASSERT_EQ( st.st_nlink, st_out.st_nlink );
   ASSERT_EQ( st.st_size, st_out.st_size );
   ASSERT_EQ( st.st_blocks, st_out.st_blocks );

}


// Test the roundtripping of a PathNode
void test_get_attributes_req_roundtrip( const std::string& n ) {
  RoundtripStreams o;
  auto path_back = std::string{};

  encode_get_attributes_req( o.out, n.c_str() );
  decode_get_attributes_req( o.in_buffered, path_back );

  ASSERT_EQ( n, path_back );
}


TEST(TFSCapnp, EncodeReaddirReq)
{
  test_readdir_req_roundtrip(  "/" );
  test_readdir_req_roundtrip( "/sitename" );
  test_readdir_req_roundtrip( "/sitename/projectname" );
  test_readdir_req_roundtrip( "/sitename/projectname/filename"  );
}

TEST(TFSCapnp, EncodeReaddirResp)
{
  test_readdir_resp_roundtrip( DirectoryList{{ "."}, {".."}} );
  test_readdir_resp_roundtrip( DirectoryList{{ "."}, {".."}, {"Default"}} );
}

TEST(TFSCapnp, EncodeGetAttributesReq)
{
  test_get_attributes_req_roundtrip(  "/" );
  test_get_attributes_req_roundtrip( "/sitename" );
  test_get_attributes_req_roundtrip( "/sitename/projectname" );
  test_get_attributes_req_roundtrip( "/sitename/projectname/filename"  );
}

TEST(TFSCapnp, EncodeGetAttributesRespNotFile)
{
  // something not a file
  auto st = Stat{};
  time( &st.st_mtime );
  st.st_size   = BlockSize;
  st.st_mode = 0555;
  st.st_nlink = 2;
  st.st_blocks = 1;
  st.st_blksize = BlockSize;
  test_getattrs_resp_roundtrip(st);
}

TEST(TFSCapnp, EncodeGetAttributesRespFile)
{
  // something not a file
  auto st = Stat{};
  time( &st.st_mtime );
  st.st_blksize = BlockSize;
  st.st_mode = 0444;
  st.st_nlink = 2;
  st.st_size   = 55 * 1024;
  st.st_blocks = (st.st_size / st.st_blksize ) + 1;
  test_getattrs_resp_roundtrip(st);
}

