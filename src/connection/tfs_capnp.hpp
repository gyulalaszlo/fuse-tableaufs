#pragma once

#include <string>
#include <sys/stat.h>

#include "connection_types.hpp"
#include "capnp/tfs.capnp.h"

namespace kj
{
  class OutputStream;
  class BufferedInputStream;
}

namespace tableauFS
{
#define CAPNP_ENCODE_FN(message, ...) \
  void encode_##message(kj::OutputStream& out, __VA_ARGS__)

#define CAPNP_DECODE_FN(message, ...) \
  void decode_##message(kj::BufferedInputStream& in, __VA_ARGS__)

#define CAPNP_ENCODE_RESP(message, ...) \
  void encode_##message(kj::OutputStream& out, __VA_ARGS__)

#define CAPNP_DECODE_RESP(message, ...) \
  ErrorCode decode_##message(kj::BufferedInputStream& in, __VA_ARGS__)

  CAPNP_ENCODE_FN(read_dir_req, const std::string& path);
  CAPNP_DECODE_FN(read_dir_req, std::string& path);

  CAPNP_ENCODE_RESP(read_dir_resp, const DirectoryList& path);
  CAPNP_DECODE_RESP(read_dir_resp, DirectoryList& path);

  CAPNP_ENCODE_FN(get_attributes_req, const std::string& path);
  CAPNP_DECODE_FN(get_attributes_req, std::string& path);

  CAPNP_ENCODE_RESP(get_attributes_resp, const struct stat& st);
  CAPNP_DECODE_RESP(get_attributes_resp, struct stat& st);
}
