#pragma once

#include <iostream>
#include <vector>

namespace cpp_utils {

  // Utility function to print a vector
  // NOTE: use fmt::println instead!
  // this is just kept as a placeholder for a future utility function!
  template <typename T>
  void print_vector(const std::vector<T>& vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
      std::cout << vec[i];
      if (i < vec.size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << "]" << std::endl;
  }

}  // namespace cpp_utils
