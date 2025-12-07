#pragma once

namespace cpp_utils {
  template <typename T>
  Coords2D<T> Coords2D<T>::step_towards_direction(Direction direction) const {
    switch (direction) {
      case Direction::East:
        return Coords2D<T>{(*this)[0], (*this)[1] + 1};
      case Direction::SouthEast:
        return Coords2D<T>{(*this)[0] + 1, (*this)[1] + 1};
      case Direction::South:
        return Coords2D<T>{(*this)[0] + 1, (*this)[1]};
      case Direction::SouthWest:
        return Coords2D<T>{(*this)[0] + 1, (*this)[1] - 1};
      case Direction::West:
        return Coords2D<T>{(*this)[0], (*this)[1] - 1};
      case Direction::NorthWest:
        return Coords2D<T>{(*this)[0] - 1, (*this)[1] - 1};
      case Direction::North:
        return Coords2D<T>{(*this)[0] - 1, (*this)[1]};
      case Direction::NorthEast:
        return Coords2D<T>{(*this)[0] - 1, (*this)[1] + 1};
      default:
        throw std::invalid_argument("Invalid direction");
    }
  }

  template <typename T>
  cpp_utils::Coords2D<T> step_into_direction(const cpp_utils::Coords2D<T> start_coord,
                                             const cpp_utils::Coords2D<T> direction,
                                             const int num_steps) {
    return start_coord + (direction * num_steps);
  }

  template <typename T>
  auto normalize_direction(Coords2D<T> const direction) {
    auto const gcd = std::gcd(direction[0], direction[1]);
    return Coords2D{direction[0] / gcd, direction[1] / gcd};
  }

  template <typename T>
  std::vector<Coords2D<T>> get_all_coords_in_line(Coords2D<T> const start_coord,
                                                  Coords2D<T> const direction,
                                                  std::function<bool(Coords2D<T>)> valid_fn) {
    std::vector<Coords2D<T>> coords;

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

  template <typename T>
  std::array<Coords2D<T>, 4> get_direct_neighbour_coords(Coords2D<T> const coords) {
    return {coords + Coords2D<T>{0, 1}, coords + Coords2D<T>{1, 0}, coords + Coords2D<T>{0, -1},
            coords + Coords2D<T>{-1, 0}};
  }

}  // namespace cpp_utils
