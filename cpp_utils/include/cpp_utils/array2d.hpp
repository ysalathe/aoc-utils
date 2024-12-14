#pragma once

#include <fmt/core.h>
#include <fmt/format.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <map>
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

  using Coords = std::pair<int, int>;

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

  Direction turn_right_90_degrees(Direction direction) {
    switch (direction) {
      case Direction::East:
        return Direction::South;
      case Direction::South:
        return Direction::West;
      case Direction::West:
        return Direction::North;
      case Direction::North:
        return Direction::East;
      case Direction::SouthEast:
        return Direction::SouthWest;
      case Direction::SouthWest:
        return Direction::NorthWest;
      case Direction::NorthWest:
        return Direction::NorthEast;
      case Direction::NorthEast:
        return Direction::SouthEast;
      default:
        throw std::invalid_argument("Invalid direction");
    }
  }

  // abstract base class for 2D arrays
  template <typename T>
  class Array2DBase {
   public:
    static constexpr Direction default_direction = Direction::East;
    static constexpr bool default_flatten = false;

    // using reference = std::vector<T>::reference;

    // Exceptions
    class DiagonalFlattenNotImplemented : public std::logic_error {
     public:
      DiagonalFlattenNotImplemented()
          : std::logic_error("Combination of diagonal direction and flatten not yet implemented.") {
      }
    };

    Array2DBase(int num_rows, int num_columns) : num_rows_(num_rows), num_columns_(num_columns) {}

    virtual T& operator()(int row, int col) = 0;
    virtual T const& operator()(int row, int col) const = 0;

    virtual T& operator()(Coords coords) = 0;
    virtual T const& operator()(Coords coords) const = 0;

    size_t num_rows() const { return num_rows_; }
    size_t num_columns() const { return num_columns_; }

    bool valid_index(int row, int col) const {
      return row >= 0 && row < num_rows_ && col >= 0 && col < num_columns_;
    }
    bool valid_index(Coords coords) const { return valid_index(coords.first, coords.second); }

    Coords upper_left_corner() const { return {0, 0}; }
    Coords upper_right_corner() const { return {0, num_columns_ - 1}; }
    Coords lower_left_corner() const { return {num_rows_ - 1, 0}; }
    Coords lower_right_corner() const { return {num_rows_ - 1, num_columns_ - 1}; }

    const Coords step_coords_towards_direction(Coords coords,
                                               Direction direction,
                                               bool flatten = false) const {
      Coords result = coords;
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
      using reference = typename std::conditional_t<IsConst,
                                                    typename std::vector<T>::const_reference,
                                                    typename std::vector<T>::reference>;
      using container_reference =
          typename std::conditional_t<IsConst, Array2DBase<T> const&, Array2DBase<T>&>;
      using container_pointer =
          typename std::conditional_t<IsConst, Array2DBase<T> const*, Array2DBase<T>*>;

      MyIterator(container_reference array,
                 Coords starting_point,
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
      Coords coords_;
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
      MyRange(container_reference array, Coords start_coords, Direction direction, bool flatten)
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

      Coords start_coords() const { return start_coords_; }
      Coords end_coords() const {
        if (flatten_) {
          return array_->flatten_end_coords(direction_);
        } else {
          return array_->end_coords(start_coords_, direction_);
        }
      }

     private:
      container_pointer array_;
      Coords start_coords_;
      Direction direction_;
      bool flatten_;
    };

    using Range = MyRange<false>;
    using ConstRange = MyRange<true>;

    Range range_from(Coords start_coords,
                     Direction direction = default_direction,
                     bool flatten = default_flatten) {
      return Range(*this, start_coords, direction, flatten);
    }

    ConstRange range_from(Coords start_coords,
                          Direction direction = default_direction,
                          bool flatten = default_flatten) const {
      return ConstRange(*this, start_coords, direction, flatten);
    }

   private:
    Coords flatten_begin_coords(Direction direction) const {
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

    Coords flatten_end_coords(Direction direction) const {
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

    Coords end_coords(Coords start_coords, Direction direction) const {
      Coords result;
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
  };

  template <typename T>
  class Array2D : virtual public Array2DBase<T> {
    using base = Array2DBase<T>;

   public:
    // Constructors
    Array2D(int num_rows, int num_columns)
        : base(num_rows, num_columns), data_(num_rows, std::vector<T>(num_columns)) {}
    Array2D(std::vector<std::vector<T>> data)
        : base(data.size(), data.at(0).size()), data_{std::move(data)} {
      assert(std::all_of(data.begin(), data.end(),
                         [this](auto const& row) { return row.size() == base::num_columns(); }));
    }
    Array2D(int num_rows, int num_columns, T const& value)
        : base(num_rows, num_columns), data_(num_rows, std::vector<T>(num_columns, value)) {}
    Array2D(int num_rows,
            int num_columns,
            std::span<const T> const& values,
            Direction direction = base::default_direction)
        : base(num_rows, num_columns), data_(num_rows, std::vector<T>(num_columns)) {
      assert(values.size() == base::num_rows() * base::num_columns());

      std::ranges::transform(values, base::begin(direction),
                             [](auto const& value) { return value; });
    }

    T& operator()(int row, int col) override { return data_.at(row).at(col); }
    T const& operator()(int row, int col) const override { return data_.at(row).at(col); }
    T& operator()(Coords coords) override { return (*this)(coords.first, coords.second); }
    T const& operator()(Coords coords) const override {
      return (*this)(coords.first, coords.second);
    }

   private:
    std::vector<std::vector<T>> data_;
  };

  template <typename T>
  class SparseElementAdapter {
   public:
    SparseElementAdapter(T const& value) : value_(value) {}

    operator T() const { return value_; }

    // assignment
    SparseElementAdapter& operator=(T const& value) {
      value_ = value;
      return *this;
    }
    bool operator==(SparseElementAdapter const& other) const { return value_ == other.value_; }

   private:
    T value_;
  };

}  // namespace cpp_utils

// Custom formatter for Array2D<T>
template <typename T>
struct fmt::formatter<cpp_utils::Array2D<T>> {
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
