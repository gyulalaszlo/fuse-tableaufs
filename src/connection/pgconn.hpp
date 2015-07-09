#pragma once

#include <string>
#include <memory>

/*
namespace tableauFS
{
  namespace connection
  {
    // Description of the host
    struct Host {
      std::string host;
      std::string port;
      std::string user;
      std::string password;
    };

    // Description of a ZMQ endpoint
    struct Endpoint {
      std::string address;
    };

    // The state of the connection (opaque for now)
    struct State;

    // Wrap up an actor.
    struct Actor {
      Actor(Host host, Endpoint endpoint);
      ~Actor();

      // We love C++11 and move contructors
      Actor(const Actor& actor) = delete;
      Actor& operator=(const Actor& other) = delete;

      Actor(Actor&& actor) = default;
      Actor& operator=(Actor&& other) = default;

      // Fields
      Host host;
      Endpoint endpoint;

      // Wrap up the state
      std::unique_ptr<State> state;
    };
  }
}
*/
