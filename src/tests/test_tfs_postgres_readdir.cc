#include "gtest/gtest.h"

#include "helper_test_pgconn.hpp"


using namespace tableauFS;

TEST(TFSPostgresTest, ReadDirFileShouldFail) {
  auto pg = make_tfs_postgres();

  auto dir_cache = DirectoryList{};
  auto dir_list = pg->read_directory({PathNode::File, "Default", "Tableau Samples", "Superstore.twbx"}, dir_cache);
  ASSERT_FALSE( dir_list.status.ok() );
  ASSERT_EQ( dir_list.status.err, -ENOTDIR);
}

TEST(TFSPostgresTest, ReadDirRoot) {
  auto pg = make_tfs_postgres();

  auto dir_cache = DirectoryList{};
  auto dir_list = pg->read_directory({PathNode::Root}, dir_cache);
  ASSERT_TRUE( dir_list.status.ok() );
  ASSERT_EQ( 3, dir_list.value.size() );
  ASSERT_EQ( ".", dir_list.value[0].name );
  ASSERT_EQ( "..", dir_list.value[1].name );
  ASSERT_EQ( "Default", dir_list.value[2].name );
}


TEST(TFSPostgresTest, ReadDirSite) {
  auto pg = make_tfs_postgres();

  auto dir_cache = DirectoryList{};
  auto dir_list = pg->read_directory({PathNode::Site, "Default"}, dir_cache);
  ASSERT_TRUE( dir_list.status.ok() );
  ASSERT_EQ( 4, dir_list.value.size() );
  ASSERT_EQ( ".", dir_list.value[0].name );
  ASSERT_EQ( "..", dir_list.value[1].name );
  ASSERT_EQ( "default", dir_list.value[2].name );
  ASSERT_EQ( "Tableau Samples", dir_list.value[3].name );
}
