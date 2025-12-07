// Builders for Array2D and SparseArray2D

#pragma once

#include <cpp_utils/array2d.hpp>

namespace cpp_utils {
  template <typename T>
  class Array2DBuilder {
   public:
    static T default_converter(std::string_view);

    static Array2D<T> create_from_string(
        std::string_view input,
        std::string_view row_separator = "\n",
        std::string_view column_separator = " ",
        std::function<T(std::string_view)> converter = default_converter) {
      auto const vec =
          get_elements_from_input(std::move(input), row_separator, column_separator, converter);
      return Array2D<T>(std::move(vec));
    }

    static SparseArray2D<T> create_sparse_from_string(
        std::string_view input,
        T empty_element,
        std::string_view row_separator = "\n",
        std::string_view column_separator = " ",
        std::function<T(std::string_view)> converter = default_converter) {
      auto const vec =
          get_elements_from_input(std::move(input), row_separator, column_separator, converter);
      return SparseArray2D<T>(std::move(vec), empty_element);
    }

   private:
    static std::vector<std::vector<T>> get_elements_from_input(
        std::string_view input,
        std::string_view row_separator,
        std::string_view column_separator,
        std::function<T(std::string_view)> converter) {
      auto lines = cpp_utils::splitString(input, row_separator);
      std::vector<std::vector<T>> result;
      std::ranges::transform(
          lines | std::views::filter([](auto const& line) { return !line.empty(); }),
          std::back_inserter(result), [column_separator, converter](std::string_view line) {
            std::vector<T> row;
            if (column_separator.empty()) {
              std::transform(line.begin(), line.end(), std::back_inserter(row),
                             [converter](char c) { return converter(std::string(1, c)); });
            } else {
              auto elements = cpp_utils::splitString(line, column_separator);
              std::ranges::transform(elements, std::back_inserter(row), converter);
            }
            return row;
          });
      return result;
    }
  };

}  // namespace cpp_utils

#include "_template_definitions/array2d_builder.tpp"