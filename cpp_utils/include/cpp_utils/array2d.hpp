#pragma once

#include <cpp_utils/input.hpp>
#include <fmt/core.h>
#include <fmt/format.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iterator>
#include <optional>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace cpp_utils {

  struct Sentinel {};

  using Coords = std::pair<int, int>;

  struct CoordsHash {
    std::size_t operator()(Coords const& coords) const {
      return std::hash<int>{}(coords.first) ^ std::hash<int>{}(coords.second);
    }
  };

  struct CoordsEqual {
    bool operator()(Coords const& lhs, Coords const& rhs) const {
      return lhs.first == rhs.first && lhs.second == rhs.second;
    }
  };

  using CoordsSet = std::unordered_set<Coords, CoordsHash, CoordsEqual>;

  template <typename T>
  using CoordsMap = std::unordered_map<Coords, T, CoordsHash, CoordsEqual>;

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

  // Abstract base class for 2D arrays
  template <typename T>
  class Array2DBase {
   public:
    static constexpr Direction default_direction = Direction::East;
    static constexpr bool default_flatten = false;

    using reference = typename std::vector<T>::reference;
    using const_reference = typename std::vector<T>::const_reference;

    // Exceptions
    class DiagonalFlattenNotImplemented : public std::logic_error {
     public:
      DiagonalFlattenNotImplemented()
          : std::logic_error("Combination of diagonal direction and flatten not yet implemented.") {
      }
    };

    Array2DBase(size_t num_rows, size_t num_columns)
        : num_rows_(num_rows), num_columns_(num_columns) {}
    virtual ~Array2DBase() = default;

    virtual reference operator()(int row, int col) = 0;
    virtual const_reference operator()(int row, int col) const = 0;

    virtual reference operator()(Coords coords) = 0;
    virtual const_reference operator()(Coords coords) const = 0;

    size_t num_rows() const { return num_rows_; }
    size_t num_columns() const { return num_columns_; }

    bool valid_index(int row, int col) const {
      return row >= 0 && row < num_rows_ && col >= 0 && col < num_columns_;
    }
    bool valid_index(Coords coords) const { return valid_index(coords.first, coords.second); }

    Coords upper_left_corner() const { return Coords{0, 0}; }
    Coords upper_right_corner() const { return Coords{0, num_columns_ - 1}; }
    Coords lower_left_corner() const { return Coords{num_rows_ - 1, 0}; }
    Coords lower_right_corner() const { return Coords{num_rows_ - 1, num_columns_ - 1}; }

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
        default:
          throw std::invalid_argument("Invalid direction");
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

    size_t num_rows_;
    size_t num_columns_;
  };

  template <typename T>
  class Array2D : public Array2DBase<T> {
    using base = Array2DBase<T>;

   public:
    // Constructors
    Array2D(size_t num_rows, size_t num_columns)
        : base(num_rows, num_columns), data_(num_rows, std::vector<T>(num_columns)) {}
    Array2D(std::vector<std::vector<T>> data)
        : base(data.size(), data.at(0).size()), data_{std::move(data)} {
      assert(std::all_of(data.begin(), data.end(),
                         [this](auto const& row) { return row.size() == base::num_columns(); }));
    }
    Array2D(size_t num_rows, size_t num_columns, T const& value)
        : base(num_rows, num_columns), data_(num_rows, std::vector<T>(num_columns, value)) {}
    Array2D(size_t num_rows,
            size_t num_columns,
            std::span<const T> const& values,
            Direction direction = base::default_direction)
        : base(num_rows, num_columns), data_(num_rows, std::vector<T>(num_columns)) {
      assert(values.size() == base::num_rows() * base::num_columns());

      std::ranges::transform(values, base::begin(direction),
                             [](auto const& value) { return value; });
    }

    typename base::reference operator()(int row, int col) override { return data_.at(row).at(col); }
    typename base::const_reference operator()(int row, int col) const override {
      return data_.at(row).at(col);
    }

    typename base::reference operator()(Coords coords) override {
      return (*this)(coords.first, coords.second);
    }
    typename base::const_reference operator()(Coords coords) const override {
      return (*this)(coords.first, coords.second);
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
        : base(num_rows, num_columns), empty_element_(empty_element) {}
    SparseArray2D(std::vector<std::vector<T>> data, T empty_element)
        : base(data.size(), data.at(0).size()), empty_element_(empty_element) {
      for (auto const [row, row_data] : std::views::enumerate(data)) {
        for (auto const [col, value] : std::views::enumerate(row_data)) {
          if (value != empty_element) {
            data_[Coords{row, col}] = value;
          }
        }
      }
    }
    SparseArray2D(int num_rows,
                  int num_columns,
                  std::span<const T> const& values,
                  T empty_element,
                  Direction direction = base::default_direction)
        : base(num_rows, num_columns), empty_element_(empty_element) {
      assert(values.size() == base::num_rows() * base::num_columns());

      std::ranges::transform(values, base::begin(direction),
                             [](auto const& value) { return value; });
      // make sure the array is cleaned up
      cleanup();
    }

    T& operator()(int row, int col) override {
      cleanup();
      auto coords = Coords{row, col};
      auto [_, inserted] = data_.emplace(coords, empty_element_);
      if (inserted) {
        cleanup_coords_ = coords;
      }
      return data_[coords];
    }
    T const& operator()(int row, int col) const override {
      auto coords = Coords{row, col};
      if (!data_.contains(coords)) {
        return empty_element_;
      }
      return data_.at(coords);
    }
    T& operator()(Coords coords) override { return (*this)(coords.first, coords.second); }
    T const& operator()(Coords coords) const override {
      return (*this)(coords.first, coords.second);
    }

    T const& empty_element() const { return empty_element_; }

    size_t size() const { return data_.size(); }

    void cleanup() {
      if (cleanup_coords_.has_value() && data_.contains(*cleanup_coords_) &&
          data_.at(*cleanup_coords_) == empty_element_) {
        data_.erase(*cleanup_coords_);
      }
      cleanup_coords_.reset();
    }

   private:
    CoordsMap<T> data_;
    T empty_element_;
    std::optional<Coords> cleanup_coords_;
  };

  // Builders for Array2D and SparseArray2D
  template <typename T>
  class Array2DBuilder {
   public:
    static const std::function<T(std::string)> default_converter;

    static Array2D<T> create_from_string(
        std::string input,
        std::string row_separator = "\n",
        std::string column_separator = " ",
        std::function<T(std::string)> converter = default_converter) {
      auto const vec =
          get_elements_from_input(std::move(input), row_separator, column_separator, converter);
      return Array2D<T>(std::move(vec));
    }

    static SparseArray2D<T> create_sparse_from_string(
        std::string input,
        T empty_element,
        std::string row_separator = "\n",
        std::string column_separator = " ",
        std::function<T(std::string)> converter = default_converter) {
      auto const vec =
          get_elements_from_input(std::move(input), row_separator, column_separator, converter);
      return SparseArray2D<T>(std::move(vec), empty_element);
    }

   private:
    static std::vector<std::vector<T>> get_elements_from_input(
        std::string input,
        std::string row_separator,
        std::string column_separator,
        std::function<T(std::string)> converter) {
      auto lines = cpp_utils::split_string(input, row_separator);
      std::vector<std::vector<T>> result;
      std::ranges::transform(
          lines | std::views::filter([](auto const& line) { return !line.empty(); }),
          std::back_inserter(result), [column_separator, converter](std::string const& line) {
            std::vector<T> row;
            if (column_separator.empty()) {
              std::transform(line.begin(), line.end(), std::back_inserter(row),
                             [converter](char c) { return converter(std::string(1, c)); });
            } else {
              auto elements = cpp_utils::split_string(line, column_separator);
              std::ranges::transform(elements, std::back_inserter(row), converter);
            }
            return row;
          });
      return result;
    }
  };

  // Define the static member outside the class
  template <typename T>
  const std::function<T(std::string)> Array2DBuilder<T>::default_converter =
      [](std::string s) { return static_cast<T>(std::stoll(s)); };

  template <>
  const std::function<char(std::string)> Array2DBuilder<char>::default_converter =
      [](std::string s) { return static_cast<char>(s[0]); };

}  // namespace cpp_utils

// Helper trait to check if a type is derived from Array2DBase
template <typename T>
struct is_array2d_base : std::false_type {};

template <typename T>
struct is_array2d_base<cpp_utils::Array2DBase<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_array2d_base_v = is_array2d_base<T>::value;

// Common formatter for classes derived from Array2DBase<T>
template <typename T>
struct array2d_formatter {
  template <typename FormatContext>
  auto format(const T& array, FormatContext& ctx) const -> decltype(ctx.out()) {
    auto result =
        fmt::format_to(ctx.out(), "Array2DBase({}x{})\n", array.num_rows(), array.num_columns());
    for (int row = 0; row < array.num_rows(); ++row) {
      auto row_range = array.range_from({row, 0});
      result = fmt::format_to(ctx.out(), "{}", fmt::join(row_range, " "));
      result = fmt::format_to(ctx.out(), "\n");
    }
    return result;
  }
};

// Custom formatter for Array2DBase<T>
template <typename T>
struct fmt::formatter<cpp_utils::Array2DBase<T>> : array2d_formatter<cpp_utils::Array2DBase<T>> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }
};

// Custom formatter for Array2D<T>
template <typename T>
struct fmt::formatter<cpp_utils::Array2D<T>> : array2d_formatter<cpp_utils::Array2D<T>> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }
};

// Custom formatter for SparseArray2D<T>
template <typename T>
struct fmt::formatter<cpp_utils::SparseArray2D<T>>
    : array2d_formatter<cpp_utils::SparseArray2D<T>> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }
};
