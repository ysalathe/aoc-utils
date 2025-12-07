#pragma once

#include <cpp_utils/array2d.hpp>

#include <charconv>
#include <span>
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
  Array2DCoords Array2DBase<T>::flatten_begin_coords(Direction direction) const {
    switch (direction) {
      case Direction::East:
        return upper_left_corner();
      case Direction::South:
        return upper_left_corner();
      case Direction::West:
        return lower_right_corner();
      case Direction::North:
        return lower_right_corner();
      default:
        throw DiagonalFlattenNotImplemented();
    }
  }

  template <typename T>
  Array2DCoords Array2DBase<T>::flatten_end_coords(Direction direction) const {
    switch (direction) {
      case Direction::East:
        return {static_cast<Array2DDim>(num_rows_), 0};
      case Direction::South:
        return {0, static_cast<Array2DDim>(num_columns_)};
      case Direction::West:
        return {-1, static_cast<Array2DDim>(num_columns_) - 1};
      case Direction::North:
        return {static_cast<Array2DDim>(num_rows_) - 1, -1};
      default:
        throw DiagonalFlattenNotImplemented();
    }
  }

  template <typename T>
  Array2DCoords Array2DBase<T>::end_coords(Array2DCoords start_coords, Direction direction) const {
    Array2DCoords result;
    switch (direction) {
      case Direction::East:
        result = {start_coords.row(), static_cast<Array2DDim>(num_columns_)};
        break;
      case Direction::South:
        result = {static_cast<Array2DDim>(num_rows_), start_coords.col()};
        break;
      case Direction::West:
        result = {start_coords.row(), -1};
        break;
      case Direction::North:
        result = {-1, start_coords.col()};
        break;
      case Direction::SouthEast: {
        int distance_to_right = num_columns_ - start_coords.col();
        int distance_to_bottom = num_rows_ - start_coords.row();
        int distance = std::min(distance_to_right, distance_to_bottom);
        result = {start_coords.row() + distance, start_coords.col() + distance};
      } break;
      case Direction::SouthWest: {
        int distance_to_left = start_coords.col() + 1;
        int distance_to_bottom = num_rows_ - start_coords.row();
        int distance = std::min(distance_to_left, distance_to_bottom);
        result = {start_coords.row() + distance, start_coords.col() - distance};
      } break;
      case Direction::NorthWest: {
        int distance_to_left = start_coords.col() + 1;
        int distance_to_top = start_coords.row() + 1;
        int distance = std::min(distance_to_left, distance_to_top);
        result = {start_coords.row() - distance, start_coords.col() - distance};
      } break;
      case Direction::NorthEast: {
        int distance_to_right = num_columns_ - start_coords.col();
        int distance_to_top = start_coords.row() + 1;
        int distance = std::min(distance_to_right, distance_to_top);
        result = {start_coords.row() - distance, start_coords.col() + distance};
      } break;
    }
    return result;
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
  SparseArray2D<T>::SparseArray2D(std::vector<std::vector<T>> data, T empty_element)
      : base({data.size(), data.at(0).size()}), empty_element_(empty_element) {
    for (auto const [row, row_data] : std::views::enumerate(data)) {
      for (auto const [col, value] : std::views::enumerate(row_data)) {
        if (value != empty_element) {
          data_[Array2DCoords{static_cast<Array2DDim>(row), Array2DDim(col)}] = value;
        }
      }
    }
  }

  template <typename T>
  SparseArray2D<T>::SparseArray2D(int num_rows,
                                  int num_columns,
                                  std::span<const T> const& values,
                                  T empty_element,
                                  Direction direction)
      : base({num_rows, num_columns}), empty_element_(empty_element) {
    assert(values.size() == base::num_rows() * base::num_columns());

    std::ranges::transform(values, base::begin(direction), [](auto const& value) { return value; });
    // make sure the array is cleaned up
    cleanup();
  }

  template <typename T>
  SparseArray2D<T>::SparseArray2D(std::tuple<size_t, size_t> dimensions,
                                  std::span<const T> const& values,
                                  T empty_element,
                                  Direction direction)
      : base(dimensions), empty_element_(empty_element) {
    assert(values.size() == base::num_rows() * base::num_columns());

    std::ranges::transform(values, base::begin(direction), [](auto const& value) { return value; });
    // make sure the array is cleaned up
    cleanup();
  }

  template <typename T>
  typename Array2DBase<T>::reference SparseArray2D<T>::operator()(size_t row, size_t col) {
    cleanup();
    auto coords = Array2DCoords{static_cast<Array2DDim>(row), Array2DDim(col)};
    auto [_, inserted] = data_.emplace(coords, empty_element_);
    if (inserted) {
      cleanup_coords_ = coords;
    }
    return data_[coords];
  }

  template <typename T>
  typename Array2DBase<T>::const_reference SparseArray2D<T>::operator()(size_t row,
                                                                        size_t col) const {
    auto coords = Array2DCoords{static_cast<Array2DDim>(row), Array2DDim(col)};
    if (!data_.contains(coords)) {
      return empty_element_;
    }
    return data_.at(coords);
  }

  template <typename T>
  void SparseArray2D<T>::cleanup() {
    if (cleanup_coords_.has_value() && data_.contains(*cleanup_coords_) &&
        data_.at(*cleanup_coords_) == empty_element_) {
      data_.erase(*cleanup_coords_);
    }
    cleanup_coords_.reset();
  }

  template <typename T>
  std::optional<Array2DCoords> SparseArray2D<T>::find_coords_of_non_empty_element_in_direction(
      Array2DCoords const& coords,
      Direction direction) const {
    switch (direction) {
      case Direction::South:
        return find_coords_of_non_empty_element_south(coords);
      case Direction::North:
        return find_coords_of_non_empty_element_north(coords);
      case Direction::East:
        return find_coords_of_non_empty_element_east(coords);
      case Direction::West:
        return find_coords_of_non_empty_element_west(coords);
      default:
        throw typename base::NotImplementedError(
            "find non-empty element in diagonal directions not yet implemented");
        return std::nullopt;
    }
  }

  template <typename T>
  std::optional<Array2DCoords> SparseArray2D<T>::find_coords_of_non_empty_element_east(
      Array2DCoords const& coords) const {
    // we can use that the map is ordered row() by the row() coordinate and then by the col()
    auto it = data_.upper_bound(coords);
    if (it == data_.end()) {
      return std::nullopt;
    }
    const auto new_coords = it->first;
    if (new_coords.row() == coords.row() && base::is_valid_index(new_coords)) {
      return new_coords;
    }
    return std::nullopt;
  }

  template <typename T>
  std::optional<Array2DCoords> SparseArray2D<T>::find_coords_of_non_empty_element_west(
      Array2DCoords const& coords) const {
    // we can use that the map is ordered row() by the row() coordinate and then by the col()
    auto it = data_.lower_bound(coords);
    it--;
    if (it == data_.end()) {
      return std::nullopt;
    }
    const auto new_coords = it->first;
    if (new_coords.row() == coords.row() && base::is_valid_index(new_coords)) {
      return new_coords;
    }
    return std::nullopt;
  }

  template <typename T>
  std::optional<Array2DCoords> SparseArray2D<T>::find_coords_of_non_empty_element_south(
      Array2DCoords const& coords) const {
    auto row = coords.row() + 1;
    auto column = coords.col();
    while (base::is_valid_index(row, column) &&
           is_empty(Array2DCoords{static_cast<Array2DDim>(row), Array2DDim(column)})) {
      row++;
    }
    if (base::is_valid_index(row, column)) {
      return Array2DCoords{static_cast<Array2DDim>(row), Array2DDim(column)};
    }
    return std::nullopt;
  }

  template <typename T>
  std::optional<Array2DCoords> SparseArray2D<T>::find_coords_of_non_empty_element_north(
      Array2DCoords const& coords) const {
    auto row = coords.row() - 1;
    auto column = coords.col();
    while (base::is_valid_index(row, column) &&
           is_empty(Array2DCoords{static_cast<Array2DDim>(row), Array2DDim(column)})) {
      row--;
    }
    if (base::is_valid_index(row, column)) {
      return Array2DCoords{static_cast<Array2DDim>(row), Array2DDim(column)};
    }
    return std::nullopt;
  }

}  // namespace cpp_utils
