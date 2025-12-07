#include <cpp_utils/array2d_builder.hpp>

namespace cpp_utils {
  template <>
  char Array2DBuilder<char>::default_converter(std::string_view s) {
    return static_cast<char>(s[0]);
  };
}  // namespace cpp_utils
