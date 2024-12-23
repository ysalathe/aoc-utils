#pragma once

#include <cmath>
#include <functional>
#include <map>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cpp_utils {

  struct Coords2D : public std::pair<int, int> {
    using std::pair<int, int>::pair;

    int& row() { return first; }
    int row() const { return first; }

    int& col() { return second; }
    int col() const { return second; }

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
    Coords2D operator*(int scalar) const { return Coords2D{first * scalar, second * scalar}; }

    // unary negation
    Coords2D operator-() const { return Coords2D{-first, -second}; }

    // division by scalar
    Coords2D operator/(int scalar) const { return Coords2D{first / scalar, second / scalar}; }
  };

  struct Coords2DHash {
    std::size_t operator()(Coords2D const& coords) const {
      return std::hash<int>{}(coords.first) ^ std::hash<int>{}(coords.second);
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
                                          const int num_steps) {
    return start_coord + (direction * num_steps);
  }

  auto normalize_direction(Coords2D const direction) {
    auto const gcd = std::gcd(direction.first, direction.second);
    return Coords2D{direction.first / gcd, direction.second / gcd};
  }

  auto get_all_coords_in_line(Coords2D const start_coord,
                              Coords2D const direction,
                              std::function<bool(Coords2D)> valid_fn) {
    std::vector<Coords2D> coords;

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
