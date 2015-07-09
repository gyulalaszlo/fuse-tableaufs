#include "tfs_capnp.hpp"

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

using namespace tableauFS;
namespace {
  // Helper to convert the enum value properly
  proto::PathNode::Level level_encode(PathNode::Level level) {
    using Lv = proto::PathNode::Level;
    using Pn = PathNode::Level;
    switch (level)
    {
      case Pn::Root: return Lv::ROOT;
      case Pn::Site: return Lv::SITE;
      case Pn::Project: return Lv::PROJECT;
      case Pn::File: return Lv::FILE;
      default: return Lv::INVALID;
    }
  }

  PathNode::Level level_decode(proto::PathNode::Level level) {
    using Lv = proto::PathNode::Level;
    using Pn = PathNode::Level;
    switch (level)
    {
      case Lv::ROOT: return Pn::Root;
      case Lv::SITE: return Pn::Site;
      case Lv::PROJECT: return Pn::Project;
      case Lv::FILE: return Pn::File;

      default: return Pn::Invalid;
    }
  }
}

namespace tableauFS {

  void encode_read_dir_req( kj::OutputStream& out, const PathNode& path  ) {
    ::capnp::MallocMessageBuilder message;

    auto root = message.initRoot<proto::ReaddirReq>();
    auto path_node = root.initPath();
    path_node.setLevel( level_encode( path.level) );
    path_node.setSite( path.site );
    path_node.setProject( path.project );
    path_node.setFile( path.file );


    writePackedMessage(out, message);
  }

  void decode_read_dir_req( kj::BufferedInputStream& in, PathNode& path )
  {
    ::capnp::PackedMessageReader message(in);

    auto root = message.getRoot<proto::ReaddirReq>();
    auto pathNode = root.getPath();
    path.level = level_decode( pathNode.getLevel() );
    path.site = pathNode.getSite();
    path.project = pathNode.getProject();
    path.file = pathNode.getFile();
  }

  void encode_read_dir_resp( kj::OutputStream& out, const DirectoryList& path  )
  {
    ::capnp::MallocMessageBuilder message;

    auto root = message.initRoot<proto::ReaddirResp>();
    auto dirs = root.initEntries(path.size());

    for (int s=path.size(), i=0; i < s; ++i) {
      dirs.set(i, path[i].name);
    }

    writePackedMessage(out, message);
  }

  void decode_read_dir_resp( kj::BufferedInputStream& in, DirectoryList& paths )
  {
    ::capnp::PackedMessageReader message(in);

    auto root = message.getRoot<proto::ReaddirResp>();

    paths.reserve( root.getEntries().size() );

    for (const auto& e : root.getEntries()) {
      paths.emplace_back( DirectoryEntry{e} );
    }
  }

}
