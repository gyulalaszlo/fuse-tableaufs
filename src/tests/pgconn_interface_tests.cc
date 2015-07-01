#include "gtest/gtest.h"

#include "connection/tfs_postgres.hpp"
#include "connection/parse_path.hpp"
#include "workgroup.h"

using namespace tableauFS;


const auto test_host = Host{ "54.203.245.18", "8060", "readonly", "onlyread" };



TEST(TFSPostgresTest, DirStatRoot) {
  auto pg = TFSPostgres::make(test_host);

  const auto path = PathNode { PathNode::Root };
  // get some stat
  auto stats = pg->get_attributes( path );
  EXPECT_TRUE( stats.status.ok() );

}


TEST(TFSPostgresTest, DirStatSite) {
  auto pg = TFSPostgres::make(test_host);

  const auto path = PathNode { PathNode::Site, "Default" };
  // get some stat
  auto stats = pg->get_attributes( path );
  EXPECT_TRUE( stats.status.ok() );

}

