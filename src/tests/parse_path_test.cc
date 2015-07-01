#include "gtest/gtest.h"

#include "connection/parse_path.hpp"

using namespace tableauFS;


TEST(PathParserTest, ParsePathInvalid1) {
  auto parsed = parse_tableau_path("/Default/test/test2asfgagag/test3");
  EXPECT_FALSE( parsed.status.ok() );
}

TEST(PathParserTest, ParsePathRoot)
{
  auto parsed = parse_tableau_path("/");
  EXPECT_TRUE( parsed.status.ok() );
  auto node = parsed.value;
  EXPECT_EQ( node.level, PathNode::Root );
}

TEST(PathParserTest, ParsePathSite)
{
  const auto parsed = parse_tableau_path("/Default");
  EXPECT_TRUE( parsed.status.ok() );
  const auto node = parsed.value;
  EXPECT_EQ( node.level, PathNode::Site );
  EXPECT_EQ( node.site, "Default" );
}

TEST(PathParserTest, ParsePathProject)
{
  const auto parsed = parse_tableau_path("/Default/test");
  EXPECT_TRUE( parsed.status.ok() );
  const auto node = parsed.value;
  EXPECT_EQ( node.level, PathNode::Project );
  EXPECT_EQ( node.site, "Default" );
  EXPECT_EQ( node.project, "test" );
}

TEST(PathParserTest, ParsePathFile)
{
  const auto parsed = parse_tableau_path("/Default/test/test_file");
  EXPECT_TRUE( parsed.status.ok() );
  const auto node = parsed.value;
  EXPECT_EQ( node.level, PathNode::File );
  EXPECT_EQ( node.site, "Default" );
  EXPECT_EQ( node.project, "test" );
  EXPECT_EQ( node.file, "test_file" );
}

