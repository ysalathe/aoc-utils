#pragma once

#include <functional>
#include <limits>

namespace cpp_utils {

  template <typename T>
    requires std::numeric_limits<T>::is_integer
  inline auto ceilDiv(T numerator, T denominator) {
    // Computes the ceiling of the division numerator / denominator
    // The argument type must be integers.
    return (numerator + denominator - 1) / denominator;
  }

  template <class T>
  inline void hashCombine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

}  // namespace cpp_utils
