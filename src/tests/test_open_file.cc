#include "gtest/gtest.h"

#include "helper_test_pgconn.hpp"


using namespace tableauFS;

TEST(TFSOpenFile, OpenDirInvalid) {
  auto pg = make_tfs_postgres();

  auto handle = pg->open_file( {PathNode::Project, "Default", "Tableau Samples"}, 0);
  //auto dir_list = pg->read_directory({PathNode::File, "Default", "Tableau Samples", "Superstore.twbx"}, dir_cache);
  ASSERT_FALSE( handle.status.ok() );
  ASSERT_EQ( 0, handle.value);
  ASSERT_EQ( -EISDIR, handle.status.err);
}


TEST(TFSOpenFile, InvalidFile) {
  auto pg = make_tfs_postgres();

  auto handle = pg->open_file( {PathNode::File, "Default", "Tableau Samples", "Invalid Superstore.twbx" }, 0);
  //auto dir_list = pg->read_directory({PathNode::File, "Default", "Tableau Samples", "Superstore.twbx"}, dir_cache);
  ASSERT_FALSE( handle.status.ok() );
  ASSERT_EQ( 0, handle.value);
  ASSERT_EQ( -ENOENT, handle.status.err);
}

TEST(TFSOpenFile, Valid) {
  auto pg = make_tfs_postgres();

  auto handle = pg->open_file( {PathNode::File, "Default", "Tableau Samples", "Superstore.twbx" }, 0);
  //auto dir_list = pg->read_directory({PathNode::File, "Default", "Tableau Samples", "Superstore.twbx"}, dir_cache);
  ASSERT_TRUE( handle.status.ok() );
  ASSERT_NE( 0, handle.value );
}

