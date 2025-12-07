
#pragma once

namespace cpp_utils {

  template <typename T>
  T Array2DBuilder<T>::default_converter(std::string_view sv) {
    T target_value;
    auto result = std::from_chars(sv.data(), sv.data() + sv.size(), target_value);
    if (result.ec != std::errc()) {
      throw std::invalid_argument("Could not convert string to target type");
    }
    return target_value;
  };

}  // namespace cpp_utils