#include "gtest/gtest.h"

#include <iostream>
#include <fstream>

#include "helper_test_pgconn.hpp"

using namespace tableauFS;

void write_test_file(const std::string& filename, slice<char> data)
{
  using namespace std;

  static const auto base_path = std::string(TFS_TEST_FILE_DIR);

  const auto full_path = base_path + filename + ".out";

  ofstream file(full_path.c_str(), ios::out | ios::binary | ios::ate);
  if (file.is_open()) {
    file.write(data.data(), data.size());
    file.close();

    cout << "the entire file content is written out from memory to "
         << full_path << " "
         << " total bytes: " << data.size() << "\n";
  } else {
    cerr << "Cannot open file:" << full_path << "\n";
    exit(-1);
  }
}

std::vector<char> read_test_file(const std::string& filename)
{
  using namespace std;

  static const auto base_path = std::string(TFS_TEST_FILE_DIR);

  const auto full_path = base_path + filename;
  auto buffer = std::vector<char>{};

  ifstream file(full_path.c_str(), ios::in | ios::binary | ios::ate);
  if (file.is_open()) {
    const auto size = file.tellg();
    buffer.resize(size);
    file.seekg(0, ios::beg);
    file.read(buffer.data(), size);
    file.close();

    cout << "the entire file content is in memory from " << full_path
         << " bytes:" << size << "\n";
  } else {
    cerr << "Cannot open file:" << full_path << "\n";
    exit(-1);
  }
  return buffer;
}

TEST(TFSReadFile, WholeFile)
{
  auto pg = make_tfs_postgres();

  const auto path = PathNode{PathNode::File, "Default", "default", "test.twb"};

  // find out the file size for the whole file read
  const auto stat = pg->get_attributes(path);
  ASSERT_TRUE(stat.status.ok());
  const auto file_size = stat.value.st_size;
  // create  a buffer for the data
  auto buffer = std::vector<char>(file_size);

  auto handle = pg->open_file(path, 0);
  ASSERT_TRUE(handle.status.ok());

  auto res = pg->read_file(handle.value, monkeykingz::make_slice(buffer),
                           file_size, 0);
  ASSERT_TRUE(res.status.ok());

  auto check_against = read_test_file(path.file);
  ASSERT_EQ(check_against.size(), file_size);

  ASSERT_EQ(0, memcmp(check_against.data(), res.value.data(), file_size));
  write_test_file(path.file, res.value);
}

// TEST(TFSReadFile, Valid) {
// auto pg = make_tfs_postgres();

// auto handle = pg->open_file( {PathNode::File, "Default", "Tableau Samples",
// "Superstore.twbx" }, 0);
// ASSERT_TRUE( handle.status.ok() );
// ASSERT_NE( 0, handle.value );
//}
