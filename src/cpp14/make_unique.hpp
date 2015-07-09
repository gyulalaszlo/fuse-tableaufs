#pragma once
#include <memory>

#ifndef HAS_CPP14
namespace std
{
  /**
   * http://herbsutter.com/2013/05/29/gotw-89-solution-smart-pointers/
   */
  template <typename T, typename... Args>
  std::unique_ptr<T> make_unique(Args&&... args)
  {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
  }
}
#endif
