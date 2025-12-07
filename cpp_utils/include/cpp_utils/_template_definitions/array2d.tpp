#pragma once

#include <cpp_utils/array2d.hpp>

#include <charconv>
#include <stdexcept>

namespace cpp_utils {

  // Iterator functions that flatten the array
  template <typename T>
  Array2DBase<T>::Iterator Array2DBase<T>::begin(Direction direction) {
    return Iterator(*this, flatten_begin_coords(direction), direction, true);
  }
  template <typename T>
  Array2DBase<T>::ConstIterator Array2DBase<T>::begin(Direction direction) const {
    return ConstIterator(*this, flatten_begin_coords(direction), direction, true);
  }
  template <typename T>
  Array2DBase<T>::Iterator Array2DBase<T>::end(Direction direction) {
    return Iterator(*this, flatten_end_coords(direction), direction, true);
  }
  template <typename T>
  Array2DBase<T>::ConstIterator Array2DBase<T>::end(Direction direction) const {
    return ConstIterator(*this, flatten_end_coords(direction), direction, true);
  }

  // Iterators for specific rows
  template <typename T>
  Array2DBase<T>::Iterator Array2DBase<T>::begin_row(size_t rowIdx) {
    if (rowIdx >= num_rows_) {
      throw std::out_of_range("Row index out of range");
    }
    return Iterator(*this, Array2DCoords{static_cast<Array2DDim>(rowIdx), 0}, Direction::East,
                    false);
  }
  template <typename T>
  Array2DBase<T>::ConstIterator Array2DBase<T>::begin_row(size_t rowIdx) const {
    if (rowIdx >= num_rows_) {
      throw std::out_of_range("Row index out of range");
    }
    return ConstIterator(*this, Array2DCoords{static_cast<Array2DDim>(rowIdx), 0}, Direction::East,
                         false);
  }
  template <typename T>
  Array2DBase<T>::Iterator Array2DBase<T>::end_row(size_t rowIdx) {
    if (rowIdx >= num_rows_) {
      throw std::out_of_range("Row index out of range");
    }
    return Iterator(*this, end_coords(Array2DCoords{static_cast<Array2DDim>(rowIdx), 0}),
                    Direction::East, false);
  }
  template <typename T>
  Array2DBase<T>::ConstIterator Array2DBase<T>::end_row(size_t rowIdx) const {
    if (rowIdx >= num_rows_) {
      throw std::out_of_range("Row index out of range");
    }
    return ConstIterator(*this, end_coords(Array2DCoords{static_cast<Array2DDim>(rowIdx), 0}),
                         Direction::East, false);
  }

  // Range functions
  template <typename T>
  Array2DBase<T>::Range Array2DBase<T>::range_from(Array2DCoords start_coords,
                                                   Direction direction,
                                                   bool flatten) {
    return Range(*this, start_coords, direction, flatten);
  }

  template <typename T>
  Array2DBase<T>::ConstRange Array2DBase<T>::range_from(Array2DCoords start_coords,
                                                        Direction direction,
                                                        bool flatten) const {
    return ConstRange(*this, start_coords, direction, flatten);
  }

  template <typename T>
  Array2DBase<T>::Range Array2DBase<T>::row_range(size_t rowIdx, int startCol) {
    return Range(*this,
                 Array2DCoords{static_cast<Array2DDim>(rowIdx), static_cast<Array2DDim>(startCol)},
                 Direction::East, false);
  }

  template <typename T>
  Array2DBase<T>::ConstRange Array2DBase<T>::row_range(size_t rowIdx, int startCol) const {
    return ConstRange(
        *this, Array2DCoords{static_cast<Array2DDim>(rowIdx), static_cast<Array2DDim>(startCol)},
        Direction::East, false);
  }

  template <typename T>
  Array2DCoords Array2DBase<T>::step_coords_towards_direction(Array2DCoords coords,
                                                              Direction direction,
                                                              bool flatten) const {
    Array2DCoords result = coords.step_towards_direction(direction);

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
