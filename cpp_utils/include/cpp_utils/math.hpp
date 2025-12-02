#pragma once

namespace cpp_utils {

  inline auto ceilDiv(auto numerator, auto denominator) {
    // Computes the ceiling of the division numerator / denominator
    // The arguments are assumed to be integers.
    return (numerator + denominator - 1) / denominator;
  }

}  // namespace cpp_utils
