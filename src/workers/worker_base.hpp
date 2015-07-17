#pragma once

#include "connection/connection_types.hpp"
#include "cpp14/markup.hpp"
#include <memory>

namespace kj
{
  class OutputStream;
  class BufferedInputStream;
}

namespace tableauFS
{
  namespace workers
  {
    class WorkerBase
    {
      MKZ_INTERFACE(WorkerBase);

      // Apply the functionnality if the stream contains the correct input.
      // otherwise return some error
      virtual int apply(kj::BufferedInputStream& input,
                        kj::OutputStream& output) = 0;
    };
  }
}
