#include "gtest/gtest.h"

#include "connection/tfs_postgres.hpp"
#include "connection/parse_path.hpp"
#include "workgroup.h"

using namespace tableauFS;


const auto test_host = Host{ "54.203.245.18", "8060", "readonly", "onlyread" };



// Tests factorial of 0.
TEST(TFSPostgresTest, DirStat) {
  auto pg = TFSPostgres::make(test_host);

  // get some stat
  auto stats = pg->get_attributes( "/Default" );
  EXPECT_TRUE( stats.status.ok() );

}

