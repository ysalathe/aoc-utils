#pragma once

#include <limits>

namespace cpp_utils {

  template <typename T>
    requires std::numeric_limits<T>::is_integer
  inline auto ceilDiv(T numerator, T denominator) {
    // Computes the ceiling of the division numerator / denominator
    // The argument type must be integers.
    return (numerator + denominator - 1) / denominator;
  }

}  // namespace cpp_utils
