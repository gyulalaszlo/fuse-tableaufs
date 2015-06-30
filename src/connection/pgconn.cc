#include "pgconn.hpp"

#include <zmqpp/zmqpp.hpp>

#include <iostream>

#include "tfs_postgres.hpp"

using std::cerr;
using std::cout;

namespace std {

  template<typename T, typename ...Args>
    std::unique_ptr<T> make_unique( Args&& ...args )
    {
      return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
    }
}

namespace {
}


namespace tableauFS {
  namespace connection {

    // Simply wrap up a ZMQ connection
    struct State {

      State(std::string address)
        : context()
        , socket( context, zmqpp::socket_type::pull )
      {
        socket.bind( address );
        cerr << "Binding to " << address << "...\n";
      }

      ~State() {
      }

      zmqpp::context context;
      zmqpp::socket socket;
    };




    Actor::Actor( Host host, Endpoint endpoint )
      : host(host)
      , endpoint(endpoint)
      , state( std::make_unique<State>(endpoint.address) )
    {
    }



    Actor::~Actor() {
      // Disconnect from PG
    }
  }
}


using namespace tableauFS::connection;

extern "C" {

  void* TFS_makeActor() { return new Actor(
      {
        Host { "54.203.245.18", "5432", "readonly", "onlyread" },
        Endpoint {"inproc://some_endpoint"}
      }
      ); }
  void TFS_destroyActor(void* actor)
  {
    delete (Actor*)actor;
  }

}
