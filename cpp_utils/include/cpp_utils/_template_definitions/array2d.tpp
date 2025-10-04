#pragma once

#include <cpp_utils/array2d.hpp>

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
          if (result.col() == num_columns()) {
            result.col() = 0;
            ++result.row();
          }
          break;
        case Direction::South:
          if (result.row() == num_rows()) {
            result.row() = 0;
            ++result.col();
            ;
          }
          break;
        case Direction::West:
          if (result.col() == -1) {
            result.col() = num_columns() - 1;
            --result.row();
          }
          break;
        case Direction::North:
          if (result.row() == -1) {
            result.row() = num_rows() - 1;
            --result.col();
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
  T Array2DBuilder<T>::default_converter(std::string_view sv) {
    T target_value;
    auto result = std::from_chars(sv.data(), sv.data() + sv.size(), target_value);
    if (result.ec != std::errc()) {
      throw std::invalid_argument("Could not convert string to target type");
    }
    return target_value;
  };

}  // namespace cpp_utils
