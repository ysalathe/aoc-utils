#pragma once

#include <fmt/core.h>
#include <fmt/format.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <optional>
#include <ranges>
#include <span>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace cpp_utils {

  struct Sentinel {};

  enum class Direction { East, SouthEast, South, SouthWest, West, NorthWest, North, NorthEast };

  Direction reverse_direction(Direction direction) {
    switch (direction) {
      case Direction::East:
        return Direction::West;
      case Direction::SouthEast:
        return Direction::NorthWest;
      case Direction::South:
        return Direction::North;
      case Direction::SouthWest:
        return Direction::NorthEast;
      case Direction::West:
        return Direction::East;
      case Direction::NorthWest:
        return Direction::SouthEast;
      case Direction::North:
        return Direction::South;
      case Direction::NorthEast:
        return Direction::SouthWest;
      default:
        throw std::invalid_argument("Invalid direction");
    }
  }

  template <typename T>
  class Array2D {
   public:
    static constexpr Direction default_direction = Direction::East;
    static constexpr bool default_flatten = false;

    // Exceptions
    class DiagonalFlattenNotImplemented : public std::logic_error {
     public:
      DiagonalFlattenNotImplemented()
          : std::logic_error("Combination of diagonal direction and flatten not yet implemented.") {
      }
    };

    // Constructors
    Array2D(int num_rows, int num_columns)
        : num_rows_(num_rows),
          num_columns_(num_columns),
          data_(num_rows, std::vector<T>(num_columns)) {}
    Array2D(std::vector<std::vector<T>> data)
        : num_rows_(data.size()), num_columns_(data.at(0).size()), data_{std::move(data)} {
      assert(std::all_of(data.begin(), data.end(),
                         [this](auto const& row) { return row.size() == num_columns_; }));
    }
    Array2D(int num_rows, int num_columns, T const& value)
        : num_rows_(num_rows),
          num_columns_(num_columns),
          data_(num_rows, std::vector<T>(num_columns, value)) {}
    Array2D(int num_rows,
            int num_columns,
            std::span<const T> const& values,
            Direction direction = default_direction)
        : num_rows_(num_rows),
          num_columns_(num_columns),
          data_(num_rows, std::vector<T>(num_columns)) {
      assert(values.size() == num_rows_ * num_columns_);

      std::ranges::transform(values, begin(direction), [](auto const& value) { return value; });
    }

    size_t num_rows() const { return num_rows_; }
    size_t num_columns() const { return num_columns_; }

    bool valid_index(int row, int col) const {
      return row >= 0 && row < num_rows_ && col >= 0 && col < num_columns_;
    }
    bool valid_index(std::pair<int, int> coords) const {
      return valid_index(coords.first, coords.second);
    }

    T& operator()(int row, int col) { return data_.at(row).at(col); }
    T& operator()(std::pair<int, int> coords) { return (*this)(coords.first, coords.second); }

    auto const& operator()(int row, int col) const { return data_.at(row).at(col); }
    auto const& operator()(std::pair<int, int> coords) const {
      return (*this)(coords.first, coords.second);
    }

    const std::pair<int, int> step_coords_towards_direction(std::pair<int, int> coords,
                                                            Direction direction,
                                                            bool flatten = false) const {
      std::pair<int, int> result = coords;
      switch (direction) {
        case Direction::East:
          ++result.second;
          if (flatten && result.second == num_columns()) {
            result.second = 0;
            ++result.first;
          }
          break;
        case Direction::SouthEast:
          ++result.first;
          ++result.second;
          break;
        case Direction::South:
          ++result.first;
          if (flatten && result.first == num_rows()) {
            result.first = 0;
            ++result.second;
            ;
          }
          break;
        case Direction::SouthWest:
          ++result.first;
          --result.second;
          break;
        case Direction::West:
          --result.second;
          if (flatten && result.second == -1) {
            result.second = num_columns() - 1;
            --result.first;
          }
          break;
        case Direction::NorthWest:
          --result.first;
          --result.second;
          break;
        case Direction::North:
          --result.first;
          if (flatten && result.first == -1) {
            result.first = num_rows() - 1;
            --result.second;
          }
          break;
        case Direction::NorthEast:
          --result.first;
          ++result.second;
          break;
      }
      return result;
    }

    template <bool IsConst>
    class MyIterator {
     public:
      using iterator_category = std::bidirectional_iterator_tag;
      using value_type = T;
      using difference_type = std::ptrdiff_t;
      using reference = typename std::conditional_t<IsConst, T const&, T&>;
      using container_reference =
          typename std::conditional_t<IsConst, Array2D<T> const&, Array2D<T>&>;
      using container_pointer =
          typename std::conditional_t<IsConst, Array2D<T> const*, Array2D<T>*>;

      MyIterator(container_reference array,
                 std::pair<int, int> starting_point,
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

      auto const& operator*() const { return (*array_)(coords_); }

      reference operator*()
        requires(!IsConst)
      {
        return (*array_)(coords_);
      }

      // Pre-increment
      MyIterator& operator++() {
        coords_ = array_->step_coords_towards_direction(coords_, direction, flatten);
        return *this;
      }

      // Post-increment
      MyIterator operator++(int) {
        MyIterator tmp = *this;
        ++(*this);
        return tmp;
      }

      // Pre-decrement
      MyIterator& operator--() {
        // Implement the reverse of step_coords_towards_direction
        coords_ =
            array_->step_coords_towards_direction(coords_, reverse_direction(direction), flatten);
        return *this;
      }

      // Post-decrement
      MyIterator operator--(int) {
        MyIterator tmp = *this;
        --(*this);
        return tmp;
      }

      MyIterator& operator+(int n) {
        for (int k = 0; k < n; ++k) {
          ++(*this);
        }
        return *this;
      }

      MyIterator& operator-(int n) {
        for (int k = 0; k < n; ++k) {
          --(*this);
        }
        return *this;
      }

      bool operator==(MyIterator const& other) const { return coords_ == other.coords_; }
      bool operator==(Sentinel const&) const { return !array_->valid_index(coords_); }

     private:
      container_pointer array_;
      std::pair<int, int> coords_;
    };

    using Iterator = MyIterator<false>;
    using ConstIterator = MyIterator<true>;

    std::pair<int, int> upper_left_corner() const { return {0, 0}; }
    std::pair<int, int> upper_right_corner() const { return {0, num_columns_ - 1}; }
    std::pair<int, int> lower_left_corner() const { return {num_rows_ - 1, 0}; }
    std::pair<int, int> lower_right_corner() const { return {num_rows_ - 1, num_columns_ - 1}; }

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
          typename std::conditional_t<IsConst, Array2D<T> const&, Array2D<T>&>;
      using container_pointer =
          typename std::conditional_t<IsConst, Array2D<T> const*, Array2D<T>*>;

     public:
      MyRange(container_reference array,
              std::pair<int, int> start_coords,
              Direction direction,
              bool flatten)
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

      std::pair<int, int> start_coords() const { return start_coords_; }
      std::pair<int, int> end_coords() const {
        if (flatten_) {
          return array_->flatten_end_coords(direction_);
        } else {
          return array_->end_coords(start_coords_, direction_);
        }
      }

     private:
      container_pointer array_;
      std::pair<int, int> start_coords_;
      Direction direction_;
      bool flatten_;
    };

    using Range = MyRange<false>;
    using ConstRange = MyRange<true>;

    Range range_from(std::pair<int, int> start_coords,
                     Direction direction = default_direction,
                     bool flatten = default_flatten) {
      return Range(*this, start_coords, direction, flatten);
    }

    ConstRange range_from(std::pair<int, int> start_coords,
                          Direction direction = default_direction,
                          bool flatten = default_flatten) const {
      return ConstRange(*this, start_coords, direction, flatten);
    }

   private:
    std::pair<int, int> flatten_begin_coords(Direction direction) const {
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

    std::pair<int, int> flatten_end_coords(Direction direction) const {
      switch (direction) {
        case Direction::East:
          return {num_rows_, 0};
        case Direction::South:
          return {0, num_columns_};
        case Direction::West:
          return {-1, num_columns_ - 1};
        case Direction::North:
          return {num_rows_ - 1, -1};
        default:
          throw DiagonalFlattenNotImplemented();
      }
    }

    std::pair<int, int> end_coords(std::pair<int, int> start_coords, Direction direction) const {
      std::pair<int, int> result;
      switch (direction) {
        case Direction::East:
          result = {start_coords.first, num_columns_};
          break;
        case Direction::South:
          result = {num_rows_, start_coords.second};
          break;
        case Direction::West:
          result = {start_coords.first, -1};
          break;
        case Direction::North:
          result = {-1, start_coords.second};
          break;
        case Direction::SouthEast: {
          int distance_to_right = num_columns_ - start_coords.second;
          int distance_to_bottom = num_rows_ - start_coords.first;
          int distance = std::min(distance_to_right, distance_to_bottom);
          result = {start_coords.first + distance, start_coords.second + distance};
        } break;
        case Direction::SouthWest: {
          int distance_to_left = start_coords.second + 1;
          int distance_to_bottom = num_rows_ - start_coords.first;
          int distance = std::min(distance_to_left, distance_to_bottom);
          result = {start_coords.first + distance, start_coords.second - distance};
        } break;
        case Direction::NorthWest: {
          int distance_to_left = start_coords.second + 1;
          int distance_to_top = start_coords.first + 1;
          int distance = std::min(distance_to_left, distance_to_top);
          result = {start_coords.first - distance, start_coords.second - distance};
        } break;
        case Direction::NorthEast: {
          int distance_to_right = num_columns_ - start_coords.second;
          int distance_to_top = start_coords.first + 1;
          int distance = std::min(distance_to_right, distance_to_top);
          result = {start_coords.first - distance, start_coords.second + distance};
        } break;
      }
      return result;
    }

    int num_rows_;
    int num_columns_;
    std::vector<std::vector<T>> data_;
  };

}  // namespace cpp_utils

// Custom formatter for Array2D<T>
template <typename T>
class fmt::formatter<cpp_utils::Array2D<T>> {
 public:
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(cpp_utils::Array2D<T> const& array, FormatContext& ctx) -> decltype(ctx.out()) const {
    auto result =
        fmt::format_to(ctx.out(), "Array2D({}x{})\n", array.num_rows(), array.num_columns());
    for (int row = 0; row < array.num_rows(); ++row) {
      auto row_range = array.range_from({row, 0});
      result = fmt::format_to(ctx.out(), "{}", fmt::join(row_range, " "));
      result = fmt::format_to(ctx.out(), "\n");
    }
    return result;
  }
};
