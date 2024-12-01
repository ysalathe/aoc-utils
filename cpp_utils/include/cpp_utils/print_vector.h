// include/cpp_utils/print_vector.h
#ifndef CPP_UTILS_PRINT_VECTOR_H
#define CPP_UTILS_PRINT_VECTOR_H

#include <iostream>
#include <vector>

namespace cpp_utils {

// Utility function to print a vector
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

} // namespace cpp_utils

#endif // CPP_UTILS_PRINT_VECTOR_H