#pragma once

#include "coords2d.hpp"
#include "input.hpp"

#include <algorithm>
#include <cassert>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iterator>
#include <map>
#include <optional>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace cpp_utils {

  struct Sentinel {};

  // Abstract base class for 2D arrays
  template <typename T>
  class Array2DBase {
   public:
    static constexpr Direction default_direction = Direction::East;
    static constexpr bool default_flatten = false;

    using reference = T&;
    using const_reference = T const&;

    // Exceptions
    class NotImplementedError : public std::logic_error {
     public:
      NotImplementedError(std::string const& message) : std::logic_error(message) {}
    };

    class DiagonalFlattenNotImplemented : NotImplementedError {
     public:
      DiagonalFlattenNotImplemented()
          : NotImplementedError(
                "Combination of diagonal direction and flatten not yet implemented.") {}
    };

    Array2DBase(std::tuple<size_t, size_t> dimensions)
        : num_rows_(std::get<0>(dimensions)), num_columns_(std::get<1>(dimensions)) {}

    virtual ~Array2DBase() = default;

    virtual reference operator()(int row, int col) = 0;
    virtual const_reference operator()(int row, int col) const = 0;

    virtual reference operator()(Coords2D coords) = 0;
    virtual const_reference operator()(Coords2D coords) const = 0;

    size_t num_rows() const { return num_rows_; }
    size_t num_columns() const { return num_columns_; }

    std::tuple<size_t, size_t> dimensions() const { return {num_rows_, num_columns_}; }

    bool is_valid_index(int row, int col) const {
      return row >= 0 && row < num_rows_ && col >= 0 && col < num_columns_;
    }
    bool is_valid_index(Coords2D coords) const {
      return is_valid_index(coords.row(), coords.col());
    }

    Coords2D upper_left_corner() const { return Coords2D{0, 0}; }
    Coords2D upper_right_corner() const {
      return Coords2D{0, static_cast<int32_t>(num_columns_) - 1};
    }
    Coords2D lower_left_corner() const { return Coords2D{static_cast<int32_t>(num_rows_) - 1, 0}; }
    Coords2D lower_right_corner() const {
      return Coords2D{static_cast<int32_t>(num_rows_) - 1, static_cast<int32_t>(num_columns_) - 1};
    }

    Coords2D step_coords_towards_direction(Coords2D coords,
                                           Direction direction,
                                           bool flatten = false) const;
    template <bool IsConst>
    class MyIterator {
     public:
      using iterator_category = std::bidirectional_iterator_tag;
      using value_type = T;
      using difference_type = int;
      using pointer = typename std::conditional_t<IsConst,
                                                  typename std::vector<T>::const_pointer,
                                                  typename std::vector<T>::pointer>;
      using reference = typename std::conditional_t<IsConst,
                                                    typename std::vector<T>::const_reference,
                                                    typename std::vector<T>::reference>;
      using container_reference =
          typename std::conditional_t<IsConst, Array2DBase<T> const&, Array2DBase<T>&>;
      using container_pointer =
          typename std::conditional_t<IsConst, Array2DBase<T> const*, Array2DBase<T>*>;

      // Default constructor
      MyIterator() = default;

      MyIterator(container_reference array,
                 Coords2D starting_point,
                 Direction direction = default_direction,
                 bool flatten = default_flatten)
          : array_(&array), coords_(starting_point), direction(direction), flatten(flatten) {
        if (flatten && (direction == Direction::NorthEast || direction == Direction::SouthWest ||
                        direction == Direction::NorthWest || direction == Direction::SouthEast)) {
          throw DiagonalFlattenNotImplemented();
        }
      }

      Direction direction;
      bool flatten;

      auto const& operator*() const {
        assert_not_null();
        return (*array_)(coords_);
      }

      reference operator*()
        requires(!IsConst)
      {
        assert_not_null();
        return (*array_)(coords_);
      }

      pointer operator->() const {
        assert_not_null();
        return &(*array_)(coords_);
      }

      auto coords() const { return coords_; }

      // Pre-increment
      MyIterator& operator++() {
        assert_not_null();
        coords_ = array_->step_coords_towards_direction(coords_, direction, flatten);
        return *this;
      }

      // Post-increment
      MyIterator operator++(difference_type) {
        assert_not_null();
        MyIterator tmp = *this;
        ++(*this);
        return tmp;
      }

      // Pre-decrement
      MyIterator& operator--() {
        assert_not_null();
        // Implement the reverse of step_coords_towards_direction
        coords_ =
            array_->step_coords_towards_direction(coords_, reverse_direction(direction), flatten);
        return *this;
      }

      // Post-decrement
      MyIterator operator--(difference_type) {
        assert_not_null();
        MyIterator tmp = *this;
        --(*this);
        return tmp;
      }

      MyIterator& operator+(difference_type n) {
        assert_not_null();
        for (difference_type k = 0; k < n; ++k) {
          ++(*this);
        }
        return *this;
      }

      MyIterator& operator-(difference_type n) {
        assert_not_null();
        for (difference_type k = 0; k < n; ++k) {
          --(*this);
        }
        return *this;
      }

      bool operator==(MyIterator const& other) const { return coords_ == other.coords_; }
      bool operator==(Sentinel const&) const {
        assert_not_null();
        return !array_->is_valid_index(coords_);
      }

     private:
      void assert_not_null() const {
        if (array_ == nullptr) {
          throw std::logic_error("Iterator is not initialized.");
        }
      }

      container_pointer array_ = NULL;
      Coords2D coords_;
    };

    using Iterator = MyIterator<false>;
    using ConstIterator = MyIterator<true>;

    Iterator begin(Direction direction = default_direction) {
      return Iterator(*this, flatten_begin_coords(direction), direction, true);
    }
    ConstIterator begin(Direction direction = default_direction) const {
      return ConstIterator(*this, flatten_begin_coords(direction), direction, true);
    }

    Iterator end(Direction direction = default_direction) {
      return Iterator(*this, flatten_end_coords(direction), direction, true);
    }
    ConstIterator end(Direction direction = default_direction) const {
      return ConstIterator(*this, flatten_end_coords(direction), direction, true);
    }

    template <bool IsConst>
    class MyRange {
      using container_reference =
          typename std::conditional_t<IsConst, Array2DBase<T> const&, Array2DBase<T>&>;
      using container_pointer =
          typename std::conditional_t<IsConst, Array2DBase<T> const*, Array2DBase<T>*>;

     public:
      MyRange(container_reference array, Coords2D start_coords, Direction direction, bool flatten)
          : array_(&array), start_coords_(start_coords), direction_(direction), flatten_(flatten) {}

      ConstIterator begin() const {
        return ConstIterator(*array_, start_coords_, direction_, flatten_);
      }

      Iterator begin()
        requires(!IsConst)
      {
        return Iterator(*array_, start_coords_, direction_, flatten_);
      }

      ConstIterator end() const {
        return ConstIterator(*array_, end_coords(), direction_, flatten_);
      }

      Iterator end()
        requires(!IsConst)
      {
        return Iterator(*array_, end_coords(), direction_, flatten_);
      }

      Coords2D start_coords() const { return start_coords_; }
      Coords2D end_coords() const {
        if (flatten_) {
          return array_->flatten_end_coords(direction_);
        } else {
          return array_->end_coords(start_coords_, direction_);
        }
      }

     private:
      container_pointer array_;
      Coords2D start_coords_;
      Direction direction_;
      bool flatten_;
    };

    using Range = MyRange<false>;
    using ConstRange = MyRange<true>;

    Range range_from(Coords2D start_coords,
                     Direction direction = default_direction,
                     bool flatten = default_flatten) {
      return Range(*this, start_coords, direction, flatten);
    }

    ConstRange range_from(Coords2D start_coords,
                          Direction direction = default_direction,
                          bool flatten = default_flatten) const {
      return ConstRange(*this, start_coords, direction, flatten);
    }

   private:
    Coords2D flatten_begin_coords(Direction direction) const {
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

    Coords2D flatten_end_coords(Direction direction) const {
      switch (direction) {
        case Direction::East:
          return {static_cast<int32_t>(num_rows_), 0};
        case Direction::South:
          return {0, static_cast<int32_t>(num_columns_)};
        case Direction::West:
          return {-1, static_cast<int32_t>(num_columns_) - 1};
        case Direction::North:
          return {static_cast<int32_t>(num_rows_) - 1, -1};
        default:
          throw DiagonalFlattenNotImplemented();
      }
    }

    Coords2D end_coords(Coords2D start_coords, Direction direction) const {
      Coords2D result;
      switch (direction) {
        case Direction::East:
          result = {start_coords.row(), static_cast<int32_t>(num_columns_)};
          break;
        case Direction::South:
          result = {static_cast<int32_t>(num_rows_), start_coords.col()};
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

    typename base::reference operator()(int row, int col) override { return data_.at(row).at(col); }
    typename base::const_reference operator()(int row, int col) const override {
      return data_.at(row).at(col);
    }

    typename base::reference operator()(Coords2D coords) override {
      return (*this)(coords.row(), coords.col());
    }
    typename base::const_reference operator()(Coords2D coords) const override {
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

    SparseArray2D(std::vector<std::vector<T>> data, T empty_element)
        : base({data.size(), data.at(0).size()}), empty_element_(empty_element) {
      for (auto const [row, row_data] : std::views::enumerate(data)) {
        for (auto const [col, value] : std::views::enumerate(row_data)) {
          if (value != empty_element) {
            data_[Coords2D{static_cast<int32_t>(row), static_cast<int32_t>(col)}] = value;
          }
        }
      }
    }

    SparseArray2D(int num_rows,
                  int num_columns,
                  std::span<const T> const& values,
                  T empty_element,
                  Direction direction = base::default_direction)
        : base({num_rows, num_columns}), empty_element_(empty_element) {
      assert(values.size() == base::num_rows() * base::num_columns());

      std::ranges::transform(values, base::begin(direction),
                             [](auto const& value) { return value; });
      // make sure the array is cleaned up
      cleanup();
    }

    SparseArray2D(std::tuple<size_t, size_t> dimensions,
                  std::span<const T> const& values,
                  T empty_element,
                  Direction direction = base::default_direction)
        : base(dimensions), empty_element_(empty_element) {
      assert(values.size() == base::num_rows() * base::num_columns());

      std::ranges::transform(values, base::begin(direction),
                             [](auto const& value) { return value; });
      // make sure the array is cleaned up
      cleanup();
    }

    typename base::reference operator()(int row, int col) override {
      cleanup();
      auto coords = Coords2D{row, col};
      auto [_, inserted] = data_.emplace(coords, empty_element_);
      if (inserted) {
        cleanup_coords_ = coords;
      }
      return data_[coords];
    }
    typename base::const_reference operator()(int row, int col) const override {
      auto coords = Coords2D{row, col};
      if (!data_.contains(coords)) {
        return empty_element_;
      }
      return data_.at(coords);
    }
    typename base::reference operator()(Coords2D coords) override {
      return (*this)(coords.row(), coords.col());
    }
    typename base::const_reference operator()(Coords2D coords) const override {
      return (*this)(coords.row(), coords.col());
    }

    typename base::const_reference empty_element() const { return empty_element_; }

    size_t size() const { return data_.size(); }

    auto const& elements() const { return data_; }

    void cleanup() {
      if (cleanup_coords_.has_value() && data_.contains(*cleanup_coords_) &&
          data_.at(*cleanup_coords_) == empty_element_) {
        data_.erase(*cleanup_coords_);
      }
      cleanup_coords_.reset();
    }

    bool is_empty(Coords2D const& coords) const {
      return !data_.contains(coords) || data_.at(coords) == empty_element_;
    }

    std::optional<Coords2D> find_coords_of_non_empty_element_in_direction(
        Coords2D const& coords,
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

    std::optional<Coords2D> find_coords_of_non_empty_element_east(Coords2D const& coords) const {
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

    std::optional<Coords2D> find_coords_of_non_empty_element_west(Coords2D const& coords) const {
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

    std::optional<Coords2D> find_coords_of_non_empty_element_south(Coords2D const& coords) const {
      auto row = coords.row() + 1;
      auto column = coords.col();
      while (base::is_valid_index(row, column) && is_empty(Coords2D{row, column})) {
        row++;
      }
      if (base::is_valid_index(row, column)) {
        return Coords2D{row, column};
      }
      return std::nullopt;
    }

    std::optional<Coords2D> find_coords_of_non_empty_element_north(Coords2D const& coords) const {
      auto row = coords.row() - 1;
      auto column = coords.col();
      while (base::is_valid_index(row, column) && is_empty(Coords2D{row, column})) {
        row--;
      }
      if (base::is_valid_index(row, column)) {
        return Coords2D{row, column};
      }
      return std::nullopt;
    }

   private:
    Coords2DMap<T> data_;
    T empty_element_;
    std::optional<Coords2D> cleanup_coords_;
  };

  // Builders for Array2D and SparseArray2D
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

// Helper trait to check if a type is derived from Array2DBase
template <typename T>
struct is_array2d_base : std::false_type {};

template <typename T>
struct is_array2d_base<cpp_utils::Array2DBase<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_array2d_base_v = is_array2d_base<T>::value;

#include "_template_definitions/array2d.tpp"
