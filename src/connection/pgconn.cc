#include "pgconn.hpp"

#include <zmqpp/zmqpp.hpp>

#include <iostream>

#include "tfs_postgres.hpp"
#include "cpp14/make_unique.hpp"

#if 0
using namespace tableauFS;

namespace
{
  using std::cerr;
  using std::cout;
  using tableauFS::PathNode;

  enum MessageType {
    Die,
    ReadDirectory
  };

  // Helpers for stream adding and reading from msg
  template <class Stream>
  Stream& operator<<( Stream& s, const PathNode& path ) {
    s << path.level << path.site << path.project << path.file;
    return s;
  }

  template <class Stream>
  Stream& operator>>( Stream& s, PathNode& path ) {
    s >> path.level;
    s >> path.site;
    s >> path.project;
    s >> path.file;
    //s << path.level << path.site << path.project << path.file;
    return s;
  }

  using GeneratorFunc = std::function<std::shared_ptr<TFSPostgres>()>;


  class TFSToZMQForward : public TFSPostgres {

    public:
    TFSToZMQForward( zmqpp::context& ctx, GeneratorFunc generator )
      : ctx(ctx)
      //, wrapped( generator() )
      , actor( make_actor(generator))
    {
    }

    ~TFSToZMQForward() {
      
    }

    private:

    zmqpp::actor make_actor( const GeneratorFunc& generator) {
      auto lambda = [&](zmqpp::socket * pipe)
      {
        auto worker = generator();
        pipe->send(zmqpp::signal::ok); // signal we successfully started

        zmqpp::message msg;
        while (true) {
          pipe->receive(msg);

          auto type = MessageType::Die;
          msg >> type;

          if (type == MessageType::Die)
            return true; // will send signal::ok to signal successful shutdown

          if (type == MessageType::ReadDirectory) {
            auto path = PathNode{PathNode::Invalid};
            msg >> path;
          }

        }

        return true; // will send signal::ok to signal successful shutdown
      };
      return zmqpp::actor(lambda);
    }

    // Tries to open a file and returns the file handle if successful
    virtual Result<FHandle> open_file(const PathNode& path, int mode)
    {
      return {NO_ERR, 0};
    }

    // Tries to list a directory and returns the list of file names
    // if successful
    virtual Result<DirectoryList&> read_directory(const PathNode& path,
                                                  DirectoryList& buffer)
    {
      auto msg = zmqpp::message(MessageType::ReadDirectory);
      msg << path;
      //msg.add_raw(&path, sizeof(path));
      actor.pipe()->send(msg);
      //actor.pipe()->receive(msg);
    }

    // Tries to stat a file
    virtual Result<struct stat> get_attributes(const PathNode& path) {}

    // Read the contents of a file into a buffer and returns the buffer itself
    virtual Result<RWBuffer> read_file(FHandle handle, RWBuffer buf,
                                       size_t size, off_t offset) {}

    // Write the contents of the buffer to the file and return the success value
    virtual Result<void> write_file(FHandle handle, ROBuffer buf,
                                    off_t offset) {}

    // Truncate...
    virtual Result<void> truncate_file(const PathNode& path, off_t offset) {}

    zmqpp::context& ctx;
    zmqpp::actor actor;
  };
}

namespace tableauFS
{
  namespace connection
  {
    // Simply wrap up a ZMQ connection
    struct State {
      State(std::string address)
          : context(), socket(context, zmqpp::socket_type::pull)
      {
        socket.bind(address);
        cerr << "Binding to " << address << "...\n";
      }

      ~State() {}

      zmqpp::context context;
      zmqpp::socket socket;
    };

    Actor::Actor(Host host, Endpoint endpoint)
        : host(host)
        , endpoint(endpoint)
        , state(std::make_unique<State>(endpoint.address))
    {
    }

    Actor::~Actor()
    {
      // Disconnect from PG
    }
  }


  std::shared_ptr<TFSPostgres>
  make_zmq_proxy (zmqpp::context& ctx, std::function<std::shared_ptr<TFSPostgres>()> generator )
  {
    return std::make_shared<TFSToZMQForward>(ctx, generator);
  }
}

using namespace tableauFS::connection;
#endif
