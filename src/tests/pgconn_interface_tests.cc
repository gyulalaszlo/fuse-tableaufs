#include "gtest/gtest.h"

#include "workgroup.h"
#include "helper_test_pgconn.hpp"

using namespace tableauFS;


TEST(TFSPostgresTest, DirStatRoot) {
  auto pg = make_tfs_postgres();
  const auto path = PathNode { PathNode::Root };
  // get some stat
  auto stats = pg->get_attributes( path );
  ASSERT_TRUE( stats.status.ok() );
  ASSERT_EQ( stats.value.st_mtime, DEFAULT_ROOT_MTIME);

}


TEST(TFSPostgresTest, DirStatSite) {
  auto pg = make_tfs_postgres();

  const auto path = PathNode { PathNode::Site, "Default" };
  // get some stat
  auto stats = pg->get_attributes( path );
  ASSERT_TRUE( stats.status.ok() );
  // for now we hardcode this mtime
  ASSERT_EQ( 946684800, stats.value.st_mtime );

}

TEST(TFSPostgresTest, DirStatInvalidSite) {
  auto pg = make_tfs_postgres();

  const auto path = PathNode { PathNode::Site, "DefaultInvalidSite" };
  // get some stat
  auto stats = pg->get_attributes( path );
  ASSERT_FALSE( stats.status.ok() );
  // for now we hardcode this mtime
  //ASSERT_EQ( stats.value.st_mtime, 946684800);

}

TEST(TFSPostgresTest, DirStatProject) {
  auto pg = make_tfs_postgres();

  const auto path = PathNode { PathNode::Project, "Default", "Tableau Samples" };
  // get some stat
  auto stats = pg->get_attributes( path );
  ASSERT_TRUE( stats.status.ok() );
  // for now we hardcode this mtime
  ASSERT_EQ( 1432796488, stats.value.st_mtime);

}

TEST(TFSPostgresTest, DirStatInvalidProject) {
  auto pg = make_tfs_postgres();

  const auto path = PathNode { PathNode::Project, "Default", "test-invalid-project" };
  // get some stat
  auto stats = pg->get_attributes( path );
  ASSERT_FALSE( stats.status.ok() );
}

// Files

TEST(TFSPostgresTest, DirStatFile) {
  auto pg = make_tfs_postgres();

  const auto path = PathNode { PathNode::File, "Default", "Tableau Samples", "Superstore.twbx" };
  // get some stat
  auto stats = pg->get_attributes( path );
  ASSERT_TRUE( stats.status.ok() );
  // for now we hardcode this mtime
  ASSERT_EQ( 1432796488, stats.value.st_mtime);
  ASSERT_EQ( 203968, stats.value.st_size);

}


TEST(TFSPostgresTest, DirStatInvalidFile) {
  auto pg = make_tfs_postgres();

  const auto path = PathNode { PathNode::File, "Default", "Tableau Samples", "Bad Superstore.twbx" };
  // get some stat
  auto stats = pg->get_attributes( path );
  ASSERT_FALSE( stats.status.ok() );
}


