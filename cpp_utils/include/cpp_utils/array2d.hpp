#pragma once

#include <optional>
#include <vector>

namespace cpp_utils {

  template <typename T>
  struct Array2D {
    int n, m;
    std::vector<std::vector<T>> data;

    Array2D(int n, int m) : n(n), m(m), data(n, std::vector<T>(m)) {}

    Array2D(int n, int m, T const& value) : n(n), m(m), data(n, std::vector<T>(m, value)) {}
    Array2D(int n, int m, std::vector<T> const& values) : n(n), m(m), data(n, std::vector<T>(m)) {
      for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
          data[i][j] = values[i * m + j];
        }
      }
    }

    bool valid_index(int i, int j) const { return i >= 0 && i < n && j >= 0 && j < m; }

    std::optional<T> operator()(int i, int j) {
      return valid_index(i, j) ? std::optional<T>{data[i][j]} : std::nullopt;
    }

    const std::optional<T> operator()(int i, int j) const {
      return valid_index(i, j) ? std::optional<T>{data[i][j]} : std::nullopt;
    }

    enum class Direction { Horizontal, Vertical, Diagonal, MinorDiagonal };

    constexpr static Direction default_direction = Direction::Horizontal;

    template <Direction dir = default_direction>
    struct Iterator {
      Array2D const* array;
      int i, j;

      Iterator(Array2D const* array, int i, int j) : array(array), i(i), j(j) {}

      std::optional<T> operator*() { return (*array)(i, j); }

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
    };

    template <Direction dir = default_direction>
    Iterator<dir> upper_left_corner() const {
      return Iterator<dir>(this, 0, 0);
    }

    template <Direction dir = default_direction>
    Iterator<dir> upper_right_corner() const {
      return Iterator<dir>(this, 0, m - 1);
    }

    template <Direction dir = default_direction>
    Iterator<dir> lower_left_corner() const {
      return Iterator<dir>(this, n - 1, 0);
    }

    template <Direction dir = default_direction>
    Iterator<dir> lower_right_corner() const {
      return Iterator<dir>(this, n - 1, m - 1);
    }

    template <Direction dir = default_direction>
    Iterator<dir> begin() const {
      return upper_left_corner<dir>();
    }
  };

}  // namespace cpp_utils