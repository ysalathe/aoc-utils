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

  struct Coords2D : public std::array<int64_t, 2> {
    using std::array<int64_t, 2>::array;

    Coords2D(int64_t row, int64_t col) : std::array<int64_t, 2>{row, col} {}

    int64_t& row() { return (*this)[0]; }
    int64_t row() const { return (*this)[0]; }

    int64_t& col() { return (*this)[1]; }
    int64_t col() const { return (*this)[1]; }

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
    Coords2D operator*(int64_t scalar) const {
      return Coords2D{(*this)[0] * scalar, (*this)[1] * scalar};
    }

    // unary negation
    Coords2D operator-() const { return Coords2D{-(*this)[0], -(*this)[1]}; }

    // division by scalar
    Coords2D operator/(int64_t scalar) const {
      return Coords2D{(*this)[0] / scalar, (*this)[1] / scalar};
    }

    // hash support
    struct Hash {
      std::size_t operator()(const Coords2D& coords) const {
        return std::hash<int64_t>{}(coords[0]) ^ std::hash<int64_t>{}(coords[1]);
      }
    };
  };

  struct Coords2DHash {
    std::size_t operator()(Coords2D const& coords) const {
      return std::hash<int64_t>{}(coords[0]) ^ std::hash<int64_t>{}(coords[1]);
    }
  };

  struct Coords2DEqual {
    bool operator()(Coords2D const& lhs, Coords2D const& rhs) const {
      return lhs[0] == rhs[0] && lhs[1] == rhs[1];
    }
  };

  struct Coords2DCompare {
    bool operator()(Coords2D const& lhs, Coords2D const& rhs) const {
      if (lhs[0] != rhs[0]) {
        return lhs[0] < rhs[0];
      } else {
        return lhs[1] < rhs[1];
      }
    }
  };

  using Coords2DUnorderedSet = std::unordered_set<Coords2D, Coords2DHash, Coords2DEqual>;

  template <typename T>
  using Coords2DUnorderedMap = std::unordered_map<Coords2D, T, Coords2DHash, Coords2DEqual>;

  template <typename T>
  using Coords2DMap = std::map<Coords2D, T, Coords2DCompare>;

  cpp_utils::Coords2D step_into_direction(const cpp_utils::Coords2D start_coord,
                                          const cpp_utils::Coords2D direction,
                                          const int64_t num_steps);

  auto normalize_direction(Coords2D const direction);

  std::vector<Coords2D> get_all_coords_in_line(Coords2D const start_coord,
                                               Coords2D const direction,
                                               std::function<bool(Coords2D)> valid_fn);

  std::array<Coords2D, 4> get_direct_neighbour_coords(Coords2D const coords);

}  // namespace cpp_utils

// Specialize std::hash for Coords2D
template <>
struct std::hash<cpp_utils::Coords2D> {
  std::size_t operator()(const cpp_utils::Coords2D& coords) const {
    return cpp_utils::Coords2D::Hash{}(coords);
  }
};
