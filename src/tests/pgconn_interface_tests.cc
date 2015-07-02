#include "gtest/gtest.h"

#include "connection/tfs_postgres.hpp"
#include "connection/tfs_postgres_config.hpp"
#include "connection/pg_connection.hpp"
#include "workgroup.h"
#include "cpp14/make_unique.hpp"

using namespace tableauFS;


enum { DEFAULT_ROOT_MTIME = 1111 };
const auto test_host = Host{ "54.203.245.18", "8060", "readonly", "onlyread" };


std::shared_ptr<TFSPostgres> make_tfs_postgres() {
  return TFSPostgres::make(
      std::make_unique<PgConnection>( test_host ),
      TFSPostgresConfig::with_static_root_mtime(DEFAULT_ROOT_MTIME)
      );
}

TEST(TFSPostgresTest, DirStatRoot) {
  auto pg = make_tfs_postgres();
  const auto path = PathNode { PathNode::Root };
  // get some stat
  auto stats = pg->get_attributes( path );
  EXPECT_TRUE( stats.status.ok() );
  EXPECT_EQ( stats.value.st_mtime, DEFAULT_ROOT_MTIME);

}


TEST(TFSPostgresTest, DirStatSite) {
  auto pg = make_tfs_postgres();

  const auto path = PathNode { PathNode::Site, "Default" };
  // get some stat
  auto stats = pg->get_attributes( path );
  EXPECT_TRUE( stats.status.ok() );
  // for now we hardcode this mtime
  EXPECT_EQ( stats.value.st_mtime, 946684800);

}

TEST(TFSPostgresTest, DirStatInvalidSite) {
  auto pg = make_tfs_postgres();

  const auto path = PathNode { PathNode::Site, "DefaultInvalidSite" };
  // get some stat
  auto stats = pg->get_attributes( path );
  EXPECT_FALSE( stats.status.ok() );
  // for now we hardcode this mtime
  //EXPECT_EQ( stats.value.st_mtime, 946684800);

}

TEST(TFSPostgresTest, DirStatProject) {
  auto pg = make_tfs_postgres();

  const auto path = PathNode { PathNode::Project, "Default", "test" };
  // get some stat
  auto stats = pg->get_attributes( path );
  EXPECT_TRUE( stats.status.ok() );
  // for now we hardcode this mtime
  EXPECT_EQ( stats.value.st_mtime, 946684800);

}

TEST(TFSPostgresTest, DirStatInvalidProject) {
  auto pg = make_tfs_postgres();

  const auto path = PathNode { PathNode::Project, "Default", "test-invalid-project" };
  // get some stat
  auto stats = pg->get_attributes( path );
  EXPECT_FALSE( stats.status.ok() );
}

