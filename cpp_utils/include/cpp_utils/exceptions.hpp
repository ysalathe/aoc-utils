// Common exceptions in utils

#pragma once

#include <stdexcept>
#include <string>

namespace cpp_utils {
  // Exceptions
  class NotImplementedError : public std::logic_error {
   public:
    NotImplementedError(std::string const& message) : std::logic_error(message) {}
  };

  class DiagonalFlattenNotImplemented : NotImplementedError {
   public:
    DiagonalFlattenNotImplemented()
        : NotImplementedError(
              "Combination of diagonal direction and flatten not yet implemented.") {}
  };
}  // namespace cpp_utils
