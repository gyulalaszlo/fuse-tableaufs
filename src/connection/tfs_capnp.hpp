#pragma once

#include "connection_types.hpp"
#include "capnp/tfs.capnp.h"

namespace kj {
  class OutputStream;
  class BufferedInputStream;
}

namespace tableauFS {

  void encode_read_dir_req( kj::OutputStream& out, const PathNode& path  );
  void decode_read_dir_req( kj::BufferedInputStream& in, PathNode& path );

  void encode_read_dir_resp( kj::OutputStream& out, const DirectoryList& path  );
  void decode_read_dir_resp( kj::BufferedInputStream& in, DirectoryList& path );

}
