#pragma once

#include "array2d.hpp"
#include "coords2d_formatter.hpp"

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

namespace _array2d_formatter_detail {

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

}  // namespace _array2d_formatter_detail

namespace fmt {

  // Disable ranges formatter for Array2DBase and its derivatives
  template <typename Char, typename T>
  struct range_format_kind<cpp_utils::Array2DBase<T>, Char>
      : std::integral_constant<range_format, range_format::disabled> {};

  template <typename Char, typename T>
  struct range_format_kind<cpp_utils::Array2D<T>, Char>
      : std::integral_constant<range_format, range_format::disabled> {};

  template <typename Char, typename T>
  struct range_format_kind<cpp_utils::SparseArray2D<T>, Char>
      : std::integral_constant<range_format, range_format::disabled> {};

  // Custom formatter for Array2DBase<T> and its derivatives
  template <typename T>
  struct formatter<cpp_utils::Array2DBase<T>>
      : _array2d_formatter_detail::array2d_formatter<cpp_utils::Array2DBase<T>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }
  };

  template <typename T>
  struct formatter<cpp_utils::Array2D<T>>
      : _array2d_formatter_detail::array2d_formatter<cpp_utils::Array2D<T>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }
  };

  template <typename T>
  struct formatter<cpp_utils::SparseArray2D<T>>
      : _array2d_formatter_detail::array2d_formatter<cpp_utils::SparseArray2D<T>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }
  };

}  // namespace fmt
