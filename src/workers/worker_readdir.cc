#include "worker_readdir.hpp"

#include <kj/io.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include "capnp/tfs.capnp.h"
#include "cpp14/make_unique.hpp"

using namespace tableauFS;
using namespace tableauFS::workers;

namespace {
  template <typename MessageT>
  std::string decode_path( kj::BufferedInputStream& in )
  {
    ::capnp::PackedMessageReader message(in);
    auto root = message.getRoot<MessageT>();
    return root.getPath();
  }


  class ReaddirImpl : public WorkerBase {
    public:
    ReaddirImpl( std::function<DirectoryList(const std::string& path)> handler )
      : handler(handler) {}
    ~ReaddirImpl() {}

    private:

    int apply(kj::BufferedInputStream& input, kj::OutputStream& output) {
      auto list = handler( decode_path<proto::ReaddirReq>(input) );

      ::capnp::MallocMessageBuilder message;

      auto root = message.initRoot<proto::ReaddirResp>();
      auto dirs = root.initEntries(list.size());

      for (int s=list.size(), i=0; i < s; ++i) {
        dirs.set(i, list[i].name);
      }

      writePackedMessage(output, message);

      return 0;
    }

    std::function<DirectoryList(const std::string& path)> handler;
  };

}

namespace tableauFS {
  namespace workers {
    std::unique_ptr<WorkerBase> make_readdir( std::function<DirectoryList(const std::string& path)> handler ) {
      return std::make_unique<ReaddirImpl>( handler );
    }

  }
}
