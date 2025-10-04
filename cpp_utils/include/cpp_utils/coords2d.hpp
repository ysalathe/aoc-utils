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

  struct Coords2D : public std::pair<int32_t, int32_t> {
    using std::pair<int32_t, int32_t>::pair;

    int32_t& row() { return first; }
    int32_t row() const { return first; }

    int32_t& col() { return second; }
    int32_t col() const { return second; }

    Coords2D step_towards_direction(Direction direction) const;

    // equality
    bool operator==(Coords2D const& other) const {
      return first == other.first && second == other.second;
    }

    // subtraction
    Coords2D operator-(Coords2D const& other) const {
      return Coords2D{first - other.first, second - other.second};
    }

    // addition
    Coords2D operator+(Coords2D const& other) const {
      return Coords2D{first + other.first, second + other.second};
    }

    // multiplication with scalar
    Coords2D operator*(int32_t scalar) const { return Coords2D{first * scalar, second * scalar}; }

    // unary negation
    Coords2D operator-() const { return Coords2D{-first, -second}; }

    // division by scalar
    Coords2D operator/(int32_t scalar) const { return Coords2D{first / scalar, second / scalar}; }

    struct Hash {
      std::size_t operator()(const Coords2D& coords) const {
        return std::hash<int32_t>{}(coords.first) ^ std::hash<int32_t>{}(coords.second);
      }
    };
  };

  struct Coords2DHash {
    std::size_t operator()(Coords2D const& coords) const {
      return std::hash<int32_t>{}(coords.first) ^ std::hash<int32_t>{}(coords.second);
    }
  };

  struct Coords2DEqual {
    bool operator()(Coords2D const& lhs, Coords2D const& rhs) const {
      return lhs.first == rhs.first && lhs.second == rhs.second;
    }
  };

  struct Coords2DCompare {
    bool operator()(Coords2D const& lhs, Coords2D const& rhs) const {
      if (lhs.first != rhs.first) {
        return lhs.first < rhs.first;
      } else {
        return lhs.second < rhs.second;
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
                                          const int32_t num_steps);

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
