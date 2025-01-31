#pragma once

#include "cpp_utils/array2d.hpp"

#include <charconv>
#include <stdexcept>

namespace cpp_utils {

  template <typename T>
  Coords2D Array2DBase<T>::step_coords_towards_direction(Coords2D coords,
                                                         Direction direction,
                                                         bool flatten) const {
    Coords2D result = coords.step_towards_direction(direction);

    if (flatten) {
      switch (direction) {
        case Direction::East:
          if (result.second == num_columns()) {
            result.second = 0;
            ++result.first;
          }
          break;
        case Direction::South:
          if (result.first == num_rows()) {
            result.first = 0;
            ++result.second;
            ;
          }
          break;
        case Direction::West:
          if (result.second == -1) {
            result.second = num_columns() - 1;
            --result.first;
          }
          break;
        case Direction::North:
          if (result.first == -1) {
            result.first = num_rows() - 1;
            --result.second;
          }
          break;
        default:
          throw DiagonalFlattenNotImplemented();
          break;
      }
    }

    return result;
  }

  template <typename T>
  const std::function<T(std::string_view)> Array2DBuilder<T>::default_converter =
      [](std::string_view sv) {
        T target_value;
        auto result = std::from_chars(sv.data(), sv.data() + sv.size(), target_value);
        if (result.ec != std::errc()) {
          throw std::invalid_argument("Could not convert string to target type");
        }
        return target_value;
      };

  template <>
  const std::function<char(std::string_view)> Array2DBuilder<char>::default_converter =
      [](std::string_view s) { return static_cast<char>(s[0]); };

  // Explicit instantiation
  template const std::function<int(std::string_view)> Array2DBuilder<int>::default_converter;

}  // namespace cpp_utils
