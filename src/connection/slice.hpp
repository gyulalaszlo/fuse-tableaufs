#pragma once

// (C) 2014 - Monkeykingz Audio
// See LICENSE for more information.
#include <cassert>
#include <type_traits>

#include "cpp14/remove_cv_t.hpp"

namespace monkeykingz
{

  /// Represents a block of homogenous stream of elements of type T
  /// in memory
  template <typename T>
  struct slice
  {
    T* values;
    size_t count;

    using value_type = T;

    friend slice<T> make_slice(T* values, size_t count) noexcept
    {
      return slice<T>{values, count};
    }

    /// Converts this slice to have a const version of the base type.
    /// This is a workaround until proper construction of slices.
    slice<const std::remove_cv_t<T>> to_const()
    {
      return slice<const std::remove_cv_t<T>>{values, count};
    }

    // Iterators
    // ---------

    friend T* begin(slice<T>& l) noexcept { return l.values; }
    friend T* end(slice<T>& l) noexcept { return l.values + l.count; }

    friend const T* begin(const slice<T>& l) noexcept { return l.values; }
    friend const T* end(const slice<T>& l) noexcept
    {
      return l.values + l.count;
    }

    friend const T* cbegin(const slice<T>& l) noexcept { return l.values; }
    friend const T* cend(const slice<T>& l) noexcept
    {
      return l.values + l.count;
    }

    // Vector-like access to data & size

    T* data() noexcept { return values; }
    const T* data() const noexcept { return values; }

    size_t size() const noexcept { return count; }

    /// Accesses an element in the slice in an array-like manner
    T& operator[](size_t idx) noexcept
    {
      assert(idx < count);
      return values[idx];
    }

    /// Accesses an element in the slice in an const array-like manner
    const T& operator[](size_t idx) const noexcept
    {
      assert(idx < count);
      return values[idx];
    }
  };

  /// Convinience wrapper for making slices
  template <typename T>
  inline slice<T> make_slice(T* values, size_t count) noexcept
  {
    return slice<T>{values, count};
  }

  /// Convinience wrapper for making slices
  template <typename T>
  inline slice<const T> make_slice(const T* values, size_t count) noexcept
  {
    return slice<const T>{values, count};
  }

  /// Convinience wrapper for making slices
  template <typename T>
  inline slice<const T> make_const_slice(const T* values, size_t count) noexcept
  {
    return slice<const T>{values, count};
  }

  /// Convinience wrapper for making slices from containers
  /// linear in memory
  template <typename Container>
  inline auto make_slice(
      Container& c) noexcept -> slice<typename Container::value_type>
  {
    return slice<typename Container::value_type>{c.data(), c.size()};
  }

  /// Convinience wrapper for making slices from containers
  /// linear in memory
  template <typename Container>
  inline auto make_slice(const Container& c) noexcept
      -> slice<const typename Container::value_type>
  {
    return slice<typename Container::value_type>{c.data(), c.size()};
  }
}
