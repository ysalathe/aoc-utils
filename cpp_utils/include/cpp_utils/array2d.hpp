#pragma once

#include <fmt/core.h>
#include <fmt/format.h>

#include <optional>
#include <vector>

namespace cpp_utils {

  template <typename T>
  class Array2D {
   public:
    Array2D(int n, int m) : n(n), m(m), data(n, std::vector<T>(m)) {}
    Array2D(int n, int m, std::vector<std::vector<T>> const& data) : n(n), m(m), data(data) {}
    Array2D(int n, int m, T const& value) : n(n), m(m), data(n, std::vector<T>(m, value)) {}
    Array2D(int n, int m, std::vector<T> const& values) : n(n), m(m), data(n, std::vector<T>(m)) {
      for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
          data[i][j] = values[i * m + j];
        }
      }
    }

    int num_rows() const { return n; };
    int num_columns() const { return m; };

    bool valid_index(int i, int j) const { return i >= 0 && i < n && j >= 0 && j < m; }

    T& operator()(int i, int j) { return data[i][j]; }

    const T& operator()(int i, int j) const { return data[i][j]; }

    enum class Direction { Horizontal, Vertical, Diagonal, MinorDiagonal };

    static constexpr Direction default_direction = Direction::Horizontal;

    class Iterator {
     public:
      using iterator_category = std::input_iterator_tag;
      using value_type = T;
      using difference_type = std::ptrdiff_t;
      using pointer = T*;
      using reference = T&;

      Iterator(Array2D const* array, int i, int j, Direction dir = default_direction)
          : array(array), i(i), j(j), dir(dir) {}

      Direction dir;

      T operator*() { return (*array)(i, j); }

      Iterator& operator++() {
        switch (dir) {
          case Direction::Horizontal:
            ++j;
            break;
          case Direction::Vertical:
            ++i;
            break;
          case Direction::Diagonal:
            ++i;
            ++j;
            break;
          case Direction::MinorDiagonal:
            ++i;
            --j;
            break;
        }
        if (!array->valid_index(i, j)) {
          i = array->num_rows();
          j = array->num_columns();
        }
        return *this;
      }

      Iterator& operator+(int n) {
        for (int k = 0; k < n; ++k) {
          ++(*this);
        }
        return *this;
      }

      bool operator!=(Iterator const& other) const { return i != other.i || j != other.j; }
      bool operator==(Iterator const& other) const { return i == other.i && j == other.j; }

     private:
      Array2D const* array;
      int i, j;
    };

    Iterator begin_upper_left_corner(Direction dir = default_direction) const {
      return Iterator(this, 0, 0, dir);
    }

    Iterator begin_upper_right_corner(Direction dir = default_direction) const {
      return Iterator(this, 0, m - 1, dir);
    }

    Iterator begin_lower_left_corner(Direction dir = default_direction) const {
      return Iterator(this, n - 1, 0, dir);
    }

    Iterator begin_lower_right_corner(Direction dir = default_direction) const {
      return Iterator(this, n - 1, m - 1, dir);
    }

    Iterator begin(Direction dir = default_direction) const { return begin_upper_left_corner(dir); }

    Iterator end() const { return Iterator(this, n, m); }

    Iterator iterator_from(int i, int j, Direction dir = default_direction) const {
      return Iterator(this, i, j, dir);
    }

   private:
    int n, m;
    std::vector<std::vector<T>> data;
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
    for (int i = 0; i < array.num_rows(); ++i) {
      for (int j = 0; j < array.num_columns(); ++j) {
        result = fmt::format_to(ctx.out(), "{} ", array(i, j));
      }
      result = fmt::format_to(ctx.out(), "\n");
    }
    return result;
  }
};