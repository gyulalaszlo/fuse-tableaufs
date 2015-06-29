#include "pgconn.hpp"

#include <zmqpp/zmqpp.hpp>

#include <iostream>

using std::cerr;
using std::cout;

namespace std {

  template<typename T, typename ...Args>
    std::unique_ptr<T> make_unique( Args&& ...args )
    {
      return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
    }
}
//int main (void)
//{
    ////  Socket to talk to clients
    //void *context = zmq_ctx_new ();
    //void *responder = zmq_socket (context, ZMQ_REP);
    //int rc = zmq_bind (responder, "tcp://*:5555");
    //assert (rc == 0);

    //while (1) {
        //char buffer [10];
        //zmq_recv (responder, buffer, 10, 0);
        //printf ("Received Hello\n");
        //sleep (1);          //  Do some 'work'
        //zmq_send (responder, "World", 5, 0);
    //}
    //return 0;
//}

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
