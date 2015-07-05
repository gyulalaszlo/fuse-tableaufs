#include "pgconn.hpp"

#include <zmqpp/zmqpp.hpp>

#include <iostream>

#include "tfs_postgres.hpp"
#include "cpp14/make_unique.hpp"

using std::cerr;
using std::cout;

namespace
{
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
}

using namespace tableauFS::connection;
