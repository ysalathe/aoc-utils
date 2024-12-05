#pragma once

#include <fmt/core.h>
#include <fmt/format.h>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <optional>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace cpp_utils {
  enum class Direction { Horizontal, Vertical, Diagonal, MinorDiagonal };
  template <typename T>
  class Array2D {
   public:
    static constexpr Direction default_direction = Direction::Horizontal;
    static constexpr bool default_flatten = false;
    static constexpr bool default_is_const = false;

    Array2D(int num_rows, int num_columns)
        : num_rows_(num_rows),
          num_columns_(num_columns),
          data_(num_rows, std::vector<T>(num_columns)) {}
    Array2D(std::vector<std::vector<T>> data)
        : num_rows_(data.size()), num_columns_(data[0].size()), data_{std::move(data)} {
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
            Direction dir = default_direction)
        : num_rows_(num_rows),
          num_columns_(num_columns),
          data_(num_rows, std::vector<T>(num_columns)) {
      assert(values.size() == num_rows_ * num_columns_);

      std::transform(values.begin(), values.end(), begin(dir, true),
                     [](auto const& value) { return value; });
    }

    size_t num_rows() const { return num_rows_; };
    size_t num_columns() const { return num_columns_; };

    bool valid_index(int row, int col) const {
      return row >= 0 && row < num_rows_ && col >= 0 && col < num_columns_;
    }

    T& operator()(int row, int col) { return data_[row][col]; }

    const T& operator()(int row, int col) const { return data_[row][col]; }

    template <bool IsConst = default_is_const>
    class MyIterator {
     public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = T;
      using difference_type = std::ptrdiff_t;
      using reference = typename std::conditional_t<IsConst, T const&, T&>;
      using pointer = typename std::conditional_t<IsConst, T const*, T*>;
      using container_pointer = typename std::conditional_t<IsConst, Array2D const*, Array2D*>;
      using container_reference = typename std::conditional_t<IsConst, Array2D const&, Array2D&>;

      MyIterator(container_reference array,
                 int row,
                 int column,
                 Direction dir = default_direction,
                 bool flatten = default_flatten)
          : array_(array), row_(row), column_(column), dir(dir), flatten(flatten) {
        if (flatten) {
          assert(dir == Direction::Horizontal || dir == Direction::Vertical);
        }
      }

      Direction dir;
      bool flatten;

      template <bool _IsConst = IsConst>
      std::enable_if_t<_IsConst, reference> operator*() const {
        return array_(row_, column_);
      }

      template <bool _IsConst = IsConst>
      std::enable_if_t<!_IsConst, reference> operator*() {
        return array_(row_, column_);
      }

      MyIterator& operator++() {
        switch (dir) {
          case Direction::Horizontal:
            ++column_;
            if (flatten && column_ == array_.num_columns()) {
              column_ = 0;
              ++row_;
            }
            break;
          case Direction::Vertical:
            ++row_;
            if (flatten && row_ == array_.num_rows()) {
              row_ = 0;
              ++column_;
            }
            break;
          case Direction::Diagonal:
            ++row_;
            ++column_;
            break;
          case Direction::MinorDiagonal:
            ++row_;
            --column_;
            break;
        }
        if (!array_.valid_index(row_, column_)) {
          // jump to the end
          row_ = array_.num_rows();
          column_ = array_.num_columns();
        }
        return *this;
      }

      MyIterator& operator+(int n) {
        for (int k = 0; k < n; ++k) {
          ++(*this);
        }
        return *this;
      }

      bool operator==(MyIterator const& other) const {
        return std::tie(row_, column_) == std::tie(other.row_, other.column_);
      }

     private:
      container_reference array_;
      int row_;
      int column_;
    };

    using Iterator = MyIterator<false>;
    using ConstIterator = MyIterator<true>;

    Iterator begin_upper_left_corner(Direction dir = default_direction,
                                     bool flatten = default_flatten) {
      return Iterator(*this, 0, 0, dir, flatten);
    }

    Iterator begin_upper_right_corner(Direction dir = default_direction,
                                      bool flatten = default_flatten) {
      return Iterator(*this, 0, num_columns_ - 1, dir, flatten);
    }

    Iterator begin_lower_left_corner(Direction dir = default_direction,
                                     bool flatten = default_flatten) {
      return Iterator(*this, num_rows_ - 1, 0, dir, flatten);
    }

    Iterator begin_lower_right_corner(Direction dir = default_direction,
                                      bool flatten = default_flatten) {
      return Iterator(*this, num_rows_ - 1, num_columns_ - 1, dir, flatten);
    }

    ConstIterator begin_upper_left_corner(Direction dir = default_direction,
                                          bool flatten = default_flatten) const {
      return ConstIterator(*this, 0, 0, dir, flatten);
    }

    ConstIterator begin_upper_right_corner(Direction dir = default_direction,
                                           bool flatten = default_flatten) const {
      return ConstIterator(*this, 0, num_columns_ - 1, dir, flatten);
    }

    ConstIterator begin_lower_left_corner(Direction dir = default_direction,
                                          bool flatten = default_flatten) const {
      return ConstIterator(*this, num_rows_ - 1, 0, dir, flatten);
    }

    ConstIterator begin_lower_right_corner(Direction dir = default_direction,
                                           bool flatten = default_flatten) const {
      return ConstIterator(*this, num_rows_ - 1, num_columns_ - 1, dir, flatten);
    }

    Iterator begin(Direction dir = default_direction, bool flatten = default_flatten) {
      return begin_upper_left_corner(dir, flatten);
    }

    Iterator end() { return Iterator(*this, num_rows_, num_columns_); }

    ConstIterator begin(Direction dir = default_direction, bool flatten = default_flatten) const {
      return begin_upper_left_corner(dir, flatten);
    }

    ConstIterator end() const { return ConstIterator(*this, num_rows_, num_columns_); }

    Iterator iterator_from(int i,
                           int j,
                           Direction dir = default_direction,
                           bool flatten = default_flatten) {
      return Iterator(*this, i, j, dir, flatten);
    }

    ConstIterator iterator_from(int i,
                                int j,
                                Direction dir = default_direction,
                                bool flatten = default_flatten) const {
      return ConstIterator(*this, i, j, dir, flatten);
    }

   private:
    int num_rows_;
    int num_columns_;
    std::vector<std::vector<T>> data_;
  };

}  // namespace cpp_utils

template <typename T>
class fmt::formatter<cpp_utils::Array2D<T>> {
 public:
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
  template <typename Context>
  constexpr auto format(cpp_utils::Array2D<T> const& array, Context& ctx) const {
    auto result =
        fmt::format_to(ctx.out(), "Array2D({}x{})\n", array.num_rows(), array.num_columns());
    for (int row = 0; row < array.num_rows(); ++row) {
      auto row_iterator = array.iterator_from(row, 0);
      result = fmt::format_to(ctx.out(), "{}",
                              fmt::join(row_iterator, row_iterator + array.num_columns(), " "));
      result = fmt::format_to(ctx.out(), "\n");
    }
    return result;
  }
};