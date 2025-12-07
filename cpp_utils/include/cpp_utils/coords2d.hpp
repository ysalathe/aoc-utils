#pragma once

#include <stdint.h>

#include <cmath>
#include <functional>
#include <map>
#include <numeric>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cpp_utils {

  enum class Direction { East, SouthEast, South, SouthWest, West, NorthWest, North, NorthEast };

  Direction reverse_direction(Direction direction);

  Direction turn_right_90_degrees(Direction direction);

  template <typename T>
  struct Coords2D : public std::array<T, 2> {
    using std::array<T, 2>::array;

    Coords2D(T row, T col) : std::array<T, 2>{row, col} {}

    T& row() { return (*this)[0]; }
    T row() const { return (*this)[0]; }

    T& col() { return (*this)[1]; }
    T col() const { return (*this)[1]; }

    Coords2D step_towards_direction(Direction direction) const;

    // equality
    bool operator==(Coords2D const& other) const {
      return (*this)[0] == other[0] && (*this)[1] == other[1];
    }

    // subtraction
    Coords2D operator-(Coords2D const& other) const {
      return Coords2D{(*this)[0] - other[0], (*this)[1] - other[1]};
    }

    // addition
    Coords2D operator+(Coords2D const& other) const {
      return Coords2D{(*this)[0] + other[0], (*this)[1] + other[1]};
    }

    // multiplication with scalar
    Coords2D operator*(T scalar) const {
      return Coords2D{(*this)[0] * scalar, (*this)[1] * scalar};
    }

    // unary negation
    Coords2D operator-() const { return Coords2D{-(*this)[0], -(*this)[1]}; }

    // division by scalar
    Coords2D operator/(T scalar) const {
      return Coords2D{(*this)[0] / scalar, (*this)[1] / scalar};
    }

    // hash support
    struct Hash {
      std::size_t operator()(const Coords2D& coords) const {
        return std::hash<T>{}(coords[0]) ^ std::hash<T>{}(coords[1]);
      }
    };
  };

  template <typename T>
  struct Coords2DHash {
    std::size_t operator()(Coords2D<T> const& coords) const {
      return std::hash<T>{}(coords[0]) ^ std::hash<T>{}(coords[1]);
    }
  };

  template <typename T>
  struct Coords2DEqual {
    bool operator()(Coords2D<T> const& lhs, Coords2D<T> const& rhs) const {
      return lhs[0] == rhs[0] && lhs[1] == rhs[1];
    }
  };

  template <typename T>
  struct Coords2DCompare {
    bool operator()(Coords2D<T> const& lhs, Coords2D<T> const& rhs) const {
      if (lhs[0] != rhs[0]) {
        return lhs[0] < rhs[0];
      } else {
        return lhs[1] < rhs[1];
      }
    }
  };

  template <typename T>
  using Coords2DUnorderedSet = std::unordered_set<Coords2D<T>, Coords2DHash<T>, Coords2DEqual<T>>;

  template <typename T, typename U>
  using Coords2DUnorderedMap =
      std::unordered_map<Coords2D<T>, U, Coords2DHash<T>, Coords2DEqual<T>>;

  template <typename T, typename U>
  using Coords2DMap = std::map<Coords2D<T>, U, Coords2DCompare<T>>;

  template <typename T>
  cpp_utils::Coords2D<T> step_into_direction(const cpp_utils::Coords2D<T> start_coord,
                                             const cpp_utils::Coords2D<T> direction,
                                             const T num_steps);

  template <typename T>
  auto normalize_direction(Coords2D<T> const direction);

  template <typename T>
  std::vector<Coords2D<T>> get_all_coords_in_line(Coords2D<T> const start_coord,
                                                  Coords2D<T> const direction,
                                                  std::function<bool(Coords2D<T>)> valid_fn);

  template <typename T>
  std::array<Coords2D<T>, 4> get_direct_neighbour_coords(Coords2D<T> const coords);

}  // namespace cpp_utils

// Specialize std::hash for Coords2D with size_t
template <>
struct std::hash<cpp_utils::Coords2D<size_t>> {
  using Coords2D = cpp_utils::Coords2D<size_t>;
  std::size_t operator()(const cpp_utils::Coords2D<size_t>& coords) const {
    return Coords2D::Hash{}(coords);
  }
};

// Specialize std::hash for Coords2D with int64_t
template <>
struct std::hash<cpp_utils::Coords2D<int64_t>> {
  using Coords2D = cpp_utils::Coords2D<int64_t>;
  std::size_t operator()(const cpp_utils::Coords2D<int64_t>& coords) const {
    return Coords2D::Hash{}(coords);
  }
};

#include "_template_definitions/coords2d.tpp"