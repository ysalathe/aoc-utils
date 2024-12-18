#pragma once

#include "array2d.hpp"
#include "coords_formatter.hpp"

#include <fmt/core.h>
#include <fmt/format.h>

namespace fmt {

  // Common formatter for classes derived from Array2DBase<T>
  template <typename T>
  struct array2d_formatter {
    template <typename FormatContext>
    auto format(const T& array, FormatContext& ctx) const -> decltype(ctx.out()) {
      auto result =
          fmt::format_to(ctx.out(), "Array2DBase({}x{})\n", array.num_rows(), array.num_columns());
      for (int row = 0; row < array.num_rows(); ++row) {
        auto row_range = array.range_from({row, 0});
        result = fmt::format_to(ctx.out(), "{}\n", fmt::join(row_range, " "));
      }
      return result;
    }
  };

  // Custom formatter for Array2DBase<T>
  template <typename T>
  struct formatter<cpp_utils::Array2DBase<T>> : array2d_formatter<cpp_utils::Array2DBase<T>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }
  };

  // Custom formatter for Array2D<T>
  template <typename T>
  struct formatter<cpp_utils::Array2D<T>> : array2d_formatter<cpp_utils::Array2D<T>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }
  };

  // Custom formatter for SparseArray2D<T>
  template <typename T>
  struct formatter<cpp_utils::SparseArray2D<T>> : array2d_formatter<cpp_utils::SparseArray2D<T>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }
  };

}  // namespace fmt
