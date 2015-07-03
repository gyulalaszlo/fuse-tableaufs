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

// Sites
// -----

TEST(TFSPostgresTest, ReadDirSite) {
  auto pg = make_tfs_postgres();

  auto dir_cache = DirectoryList{};
  auto dir_list = pg->read_directory({PathNode::Site, "Default"}, dir_cache);
  ASSERT_TRUE( dir_list.status.ok() );
  ASSERT_EQ( 4, dir_list.value.size() );
  ASSERT_EQ( ".", dir_list.value[0].name );
  ASSERT_EQ( "..", dir_list.value[1].name );
  ASSERT_EQ( "Tableau Samples", dir_list.value[2].name );
  ASSERT_EQ( "default", dir_list.value[3].name );
}

TEST(TFSPostgresTest, ReadDirInvalidSite) {
  auto pg = make_tfs_postgres();

  auto dir_cache = DirectoryList{};
  auto dir_list = pg->read_directory({PathNode::Site, "Invalid Default"}, dir_cache);
  ASSERT_FALSE( dir_list.status.ok() );
  ASSERT_EQ( -ENOENT, dir_list.status.err );
  ASSERT_EQ( 0, dir_list.value.size() );
}


// Projects
// --------

TEST(TFSPostgresTest, ReadDirProject) {
  auto pg = make_tfs_postgres();

  auto dir_cache = DirectoryList{};
  auto dir_list = pg->read_directory({PathNode::Project, "Default", "Tableau Samples"}, dir_cache);

  ASSERT_TRUE( dir_list.status.ok() );
  ASSERT_EQ( 7, dir_list.value.size() );

  // Dots
  ASSERT_EQ( ".", dir_list.value[0].name );
  ASSERT_EQ( "..", dir_list.value[1].name );

  // uppercase entries first
  ASSERT_EQ( "Dashboard Parameter Example Many Dashboards2.twbx", dir_list.value[2].name );
  ASSERT_EQ( "Regional.twbx", dir_list.value[3].name );
  ASSERT_EQ( "Superstore.twbx", dir_list.value[4].name );

  // lowercase entries second
  ASSERT_EQ( "iphone5.twbx", dir_list.value[5].name );
  ASSERT_EQ( "test.twb", dir_list.value[6].name );
}

TEST(TFSPostgresTest, ReadDirProjectInvalid) {
  auto pg = make_tfs_postgres();

  auto dir_cache = DirectoryList{};
  auto dir_list = pg->read_directory({PathNode::Project, "Default", "Invalid Tableau Samples"}, dir_cache);

  ASSERT_FALSE( dir_list.status.ok() );
  ASSERT_EQ( -ENOENT, dir_list.status.err );
  ASSERT_EQ( 0, dir_list.value.size() );
}

