#pragma once

#include "array2d.hpp"

#include <fmt/core.h>
#include <fmt/format.h>

namespace fmt {
  template <typename T>
  struct formatter<cpp_utils::Coords2D<T>> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const cpp_utils::Coords2D<T>& coords, FormatContext& ctx) {
      return fmt::format_to(ctx.out(), "({}, {})", coords.row(), coords.col());
    }
  };

}  // namespace fmt
