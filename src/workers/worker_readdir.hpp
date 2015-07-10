#pragma once

#include <functional>
#include "worker_base.hpp"


namespace tableauFS {
  namespace workers {

    std::unique_ptr<WorkerBase> make_readdir( std::function<DirectoryList(const std::string& path)> handler );

  }
}
