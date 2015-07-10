#include "tfs_capnp.hpp"

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

using namespace tableauFS;
namespace {

  template <typename MessageT>
  void encode_path( kj::OutputStream& out, const std::string& path  ) {
    ::capnp::MallocMessageBuilder message;

    auto root = message.initRoot<MessageT>();
    root.setPath( path );

    writePackedMessage(out, message);
  }

  template <typename MessageT>
  void decode_path( kj::BufferedInputStream& in, std::string& out )
  {
    ::capnp::PackedMessageReader message(in);
    auto root = message.getRoot<MessageT>();
    out = root.getPath();

  }


}

namespace tableauFS {

  CAPNP_ENCODE_FN( read_dir_req, const std::string& path ) { encode_path<proto::ReaddirReq>(out, path); }
  CAPNP_DECODE_FN( read_dir_req, std::string& path ) {  decode_path<proto::ReaddirReq>( in, path ); }


  CAPNP_ENCODE_RESP( read_dir_resp, const DirectoryList& path )
  {
    ::capnp::MallocMessageBuilder message;

    auto root = message.initRoot<proto::ReaddirResp>();
    auto dirs = root.initEntries(path.size());

    for (int s=path.size(), i=0; i < s; ++i) {
      dirs.set(i, path[i].name);
    }

    writePackedMessage(out, message);
  }


  CAPNP_DECODE_RESP( read_dir_resp, DirectoryList& paths )
  {
    ::capnp::PackedMessageReader message(in);

    auto root = message.getRoot<proto::ReaddirResp>();

    paths.reserve( root.getEntries().size() );

    for (const auto& e : root.getEntries()) {
      paths.emplace_back( DirectoryEntry{e} );
    }

    return root.getErr();
  }


  // GetAttributes

  CAPNP_ENCODE_FN( get_attributes_req, const std::string& path ) { encode_path<proto::GetAttributesReq>(out, path); }
  CAPNP_DECODE_FN( get_attributes_req, std::string& path ) {  decode_path<proto::GetAttributesReq>( in, path ); }


  CAPNP_ENCODE_RESP( get_attributes_resp, const struct stat& st ) {
    ::capnp::MallocMessageBuilder message;

    auto root = message.initRoot<proto::GetAttributesResp>();

    root.setMtime( st.st_mtime );
    root.setMode( st.st_mode );
    root.setNlink( st.st_nlink );
    root.setSize( st.st_size );
    root.setBlocks( st.st_blocks );
    root.setBlksize( st.st_blksize );


    writePackedMessage(out, message);
  }


  CAPNP_DECODE_RESP( get_attributes_resp, struct stat& st ) {
    ::capnp::PackedMessageReader message(in);

    auto root = message.getRoot<proto::GetAttributesResp>();

    st.st_mtime = root.getMtime();
    st.st_mode = root.getMode();
    st.st_nlink = root.getNlink();
    st.st_size   = root.getSize();
    st.st_blocks = root.getBlocks();
    st.st_blksize = root.getBlksize();

    return root.getErr();
  }

}
