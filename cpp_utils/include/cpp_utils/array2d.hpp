// A generic 2D array implementation with support for dense and sparse arrays,
// iterators in various directions, and utility functions.
// TODO(YSA): Idea: implement coordinate view that allows to get the coordinates instead of the
// elements of the range

#pragma once

#include "array2d_iter.hpp"
#include "array2d_range.hpp"
#include "coords2d.hpp"
#include "input.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iterator>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace cpp_utils {

  // Signed coordantes are used to represent boundaries in iterators (e.g., -1 for before the first
  // row)
  using Array2DDim = int64_t;
  using Array2DCoords = Coords2D<Array2DDim>;

  // Abstract base class for 2D arrays
  template <typename T>
  class Array2DBase {
   public:
    static constexpr Direction default_direction = Direction::East;
    static constexpr bool default_flatten = false;

    using reference = T&;
    using const_reference = T const&;

    Array2DBase(std::tuple<size_t, size_t> dimensions)
        : num_rows_{std::get<0>(dimensions)}, num_columns_{std::get<1>(dimensions)} {}

    virtual ~Array2DBase() = default;

    virtual reference operator()(size_t row, size_t col) = 0;
    virtual const_reference operator()(size_t row, size_t col) const = 0;

    virtual reference operator()(Array2DCoords coords) = 0;
    virtual const_reference operator()(Array2DCoords coords) const = 0;

    size_t num_rows() const { return num_rows_; }
    size_t num_columns() const { return num_columns_; }

    std::tuple<size_t, size_t> dimensions() const { return {num_rows_, num_columns_}; }

    bool is_valid_index(size_t row, size_t col) const {
      return row < num_rows_ && col < num_columns_;
    }
    bool is_valid_index(Array2DCoords coords) const {
      return is_valid_index(coords.row(), coords.col());
    }

    Array2DCoords upper_left_corner() const { return Array2DCoords{0, 0}; }
    Array2DCoords upper_right_corner() const {
      return Array2DCoords{0, static_cast<Array2DDim>(num_columns_) - 1};
    }
    Array2DCoords lower_left_corner() const {
      return Array2DCoords{static_cast<Array2DDim>(num_rows_) - 1, 0};
    }
    Array2DCoords lower_right_corner() const {
      return Array2DCoords{static_cast<Array2DDim>(num_rows_) - 1,
                           static_cast<Array2DDim>(num_columns_) - 1};
    }

    Array2DCoords step_coords_towards_direction(Array2DCoords coords,
                                                Direction direction,
                                                bool flatten = false) const;

    using Iterator = Array2DIterator<Array2DBase, T, false>;
    using ConstIterator = Array2DIterator<Array2DBase, T, true>;

    // Iterator functions that flatten the array
    Iterator begin(Direction direction = default_direction);
    ConstIterator begin(Direction direction = default_direction) const;
    Iterator end(Direction direction = default_direction);
    ConstIterator end(Direction direction = default_direction) const;

    // Iterators for specific rows
    Iterator begin_row(size_t rowIdx);
    ConstIterator begin_row(size_t rowIdx) const;
    Iterator end_row(size_t rowIdx);
    ConstIterator end_row(size_t rowIdx) const;

    // Range functions
    using Range = Array2DRange<Array2DBase, T, false>;
    using ConstRange = Array2DRange<Array2DBase, T, true>;

    Range range_from(Array2DCoords start_coords,
                     Direction direction = default_direction,
                     bool flatten = default_flatten);
    ConstRange range_from(Array2DCoords start_coords,
                          Direction direction = default_direction,
                          bool flatten = default_flatten) const;

    Range row_range(size_t rowIdx, int startCol = 0);
    ConstRange row_range(size_t rowIdx, int startCol = 0) const;

   private:
    friend class Array2DRange<Array2DBase, T, false>;
    friend class Array2DRange<Array2DBase, T, true>;

    Array2DCoords flatten_begin_coords(Direction direction) const;
    Array2DCoords flatten_end_coords(Direction direction) const;

    Array2DCoords end_coords(Array2DCoords start_coords, Direction direction) const;

    size_t num_rows_;
    size_t num_columns_;
  };

  template <typename T>
  class Array2D : public Array2DBase<T> {
    using base = Array2DBase<T>;

   public:
    // Constructors
    Array2D(std::tuple<size_t, size_t> dimensions)
        : base(dimensions),
          data_(std::get<0>(dimensions), std::vector<T>(std::get<1>(dimensions))) {}

    Array2D(std::vector<std::vector<T>> data)
        : base({data.size(), data.at(0).size()}), data_{std::move(data)} {
      assert(std::all_of(data.begin(), data.end(),
                         [this](auto const& row) { return row.size() == base::num_columns(); }));
    }

    Array2D(std::tuple<size_t, size_t> dimensions, T const& value)
        : base(dimensions),
          data_(std::get<0>(dimensions), std::vector<T>(std::get<1>(dimensions), value)) {}

    Array2D(std::tuple<size_t, size_t> dimensions,
            std::span<const T> const& values,
            Direction direction = base::default_direction)
        : base(dimensions),
          data_(std::get<0>(dimensions), std::vector<T>(std::get<1>(dimensions))) {
      assert(values.size() == base::num_rows() * base::num_columns());

      std::ranges::transform(values, base::begin(direction),
                             [](auto const& value) { return value; });
    }

    typename base::reference operator()(size_t row, size_t col) override {
      return data_.at(row).at(col);
    }
    typename base::const_reference operator()(size_t row, size_t col) const override {
      return data_.at(row).at(col);
    }

    typename base::reference operator()(Array2DCoords coords) override {
      return (*this)(coords.row(), coords.col());
    }
    typename base::const_reference operator()(Array2DCoords coords) const override {
      return (*this)(coords.row(), coords.col());
    }

   private:
    std::vector<std::vector<T>> data_;
  };

  template <typename T>
  class SparseArray2D : virtual public Array2DBase<T> {
    using base = Array2DBase<T>;

   public:
    // Constructors
    SparseArray2D(size_t num_rows, size_t num_columns, T empty_element)
        : base({num_rows, num_columns}), empty_element_(empty_element) {}

    SparseArray2D(std::tuple<size_t, size_t> dimensions, T empty_element)
        : base(dimensions), empty_element_(empty_element) {}

    SparseArray2D(std::vector<std::vector<T>> data, T empty_element);

    SparseArray2D(int num_rows,
                  int num_columns,
                  std::span<const T> const& values,
                  T empty_element,
                  Direction direction = base::default_direction);

    SparseArray2D(std::tuple<size_t, size_t> dimensions,
                  std::span<const T> const& values,
                  T empty_element,
                  Direction direction = base::default_direction);

    typename base::reference operator()(size_t row, size_t col) override;
    typename base::const_reference operator()(size_t row, size_t col) const override;
    typename base::reference operator()(Array2DCoords coords) override {
      return (*this)(coords.row(), coords.col());
    }
    typename base::const_reference operator()(Array2DCoords coords) const override {
      return (*this)(coords.row(), coords.col());
    }

    typename base::const_reference empty_element() const { return empty_element_; }

    size_t size() const { return data_.size(); }

    auto const& elements() const { return data_; }

    void cleanup();

    bool is_empty(Array2DCoords const& coords) const {
      return !data_.contains(coords) || data_.at(coords) == empty_element_;
    }

    std::optional<Array2DCoords> find_coords_of_non_empty_element_in_direction(
        Array2DCoords const& coords,
        Direction direction) const;

    std::optional<Array2DCoords> find_coords_of_non_empty_element_east(
        Array2DCoords const& coords) const;

    std::optional<Array2DCoords> find_coords_of_non_empty_element_west(
        Array2DCoords const& coords) const;

    std::optional<Array2DCoords> find_coords_of_non_empty_element_south(
        Array2DCoords const& coords) const;

    std::optional<Array2DCoords> find_coords_of_non_empty_element_north(
        Array2DCoords const& coords) const;

   private:
    Coords2DMap<Array2DDim, T> data_;
    T empty_element_;
    std::optional<Array2DCoords> cleanup_coords_;
  };

}  // namespace cpp_utils

// Helper trait to check if a type is derived from Array2DBase
template <typename T>
struct is_array2d_base : std::false_type {};

template <typename T>
struct is_array2d_base<cpp_utils::Array2DBase<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_array2d_base_v = is_array2d_base<T>::value;

#include "_template_definitions/array2d.tpp"
