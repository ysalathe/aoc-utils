#pragma once

#include <cmath>
#include <functional>
#include <map>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cpp_utils {

  struct Coords : public std::pair<int, int> {
    using std::pair<int, int>::pair;

    int& row() { return first; }
    int row() const { return first; }

    int& col() { return second; }
    int col() const { return second; }

    // subtraction
    Coords operator-(Coords const& other) const {
      return Coords{first - other.first, second - other.second};
    }

    // addition
    Coords operator+(Coords const& other) const {
      return Coords{first + other.first, second + other.second};
    }

    // multiplication with scalar
    Coords operator*(int scalar) const { return Coords{first * scalar, second * scalar}; }

    // unary negation
    Coords operator-() const { return Coords{-first, -second}; }

    // division by scalar
    Coords operator/(int scalar) const { return Coords{first / scalar, second / scalar}; }
  };

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

  struct CoordsCompare {
    bool operator()(Coords const& lhs, Coords const& rhs) const {
      if (lhs.first != rhs.first) {
        return lhs.first < rhs.first;
      } else {
        return lhs.second < rhs.second;
      }
    }
  };

  using CoordsUnorderedSet = std::unordered_set<Coords, CoordsHash, CoordsEqual>;

  template <typename T>
  using CoordsUnorderedMap = std::unordered_map<Coords, T, CoordsHash, CoordsEqual>;

  template <typename T>
  using CoordsMap = std::map<Coords, T, CoordsCompare>;

  cpp_utils::Coords step_into_direction(const cpp_utils::Coords start_coord,
                                        const cpp_utils::Coords direction,
                                        const int num_steps) {
    return start_coord + (direction * num_steps);
  }

  auto normalize_direction(Coords const direction) {
    auto const gcd = std::gcd(direction.first, direction.second);
    return Coords{direction.first / gcd, direction.second / gcd};
  }

  auto get_all_coords_in_line(Coords const start_coord,
                              Coords const direction,
                              std::function<bool(Coords)> valid_fn) {
    std::vector<Coords> coords;

    auto const normalized_direction = normalize_direction(direction);
    // now create valid coords in both directions starting from start_coord
    for (int i = 1; valid_fn(step_into_direction(start_coord, normalized_direction, i)); ++i) {
      coords.push_back(step_into_direction(start_coord, normalized_direction, i));
    }
    auto const neg_normalized_direction = -normalized_direction;
    for (int i = 1; valid_fn(step_into_direction(start_coord, neg_normalized_direction, i)); ++i) {
      coords.push_back(step_into_direction(start_coord, neg_normalized_direction, i));
    }
    return coords;
  }

}  // namespace cpp_utils
