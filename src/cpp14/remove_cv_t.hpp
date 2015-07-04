#pragma once

namespace std
{
  template <typename T>
  using remove_cv_t = typename std::remove_cv<T>::type;
}
