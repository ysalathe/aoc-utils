#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>

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

}  // namespace cpp_utils
