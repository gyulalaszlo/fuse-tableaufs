#pragma once

#include "connection_types.hpp"

namespace std {

  template <>
    struct hash<tableauFS::Host> {
      using argument_type = tableauFS::Host;
      using result_type = std::size_t;

      result_type operator()(argument_type const& s) const
      {
        result_type const h1 ( std::hash<std::string>()(s.first_name) );
        result_type const h2 ( std::hash<std::string>()(s.last_name) );
        return h1 ^ (h2 << 1);
      }
    };
}
