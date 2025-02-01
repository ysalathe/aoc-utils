#include <cpp_utils/coords2d.hpp>

namespace cpp_utils {
  Direction reverse_direction(Direction direction) {
    switch (direction) {
      case Direction::East:
        return Direction::West;
      case Direction::SouthEast:
        return Direction::NorthWest;
      case Direction::South:
        return Direction::North;
      case Direction::SouthWest:
        return Direction::NorthEast;
      case Direction::West:
        return Direction::East;
      case Direction::NorthWest:
        return Direction::SouthEast;
      case Direction::North:
        return Direction::South;
      case Direction::NorthEast:
        return Direction::SouthWest;
      default:
        throw std::invalid_argument("Invalid direction");
    }
  }

  Direction turn_right_90_degrees(Direction direction) {
    switch (direction) {
      case Direction::East:
        return Direction::South;
      case Direction::South:
        return Direction::West;
      case Direction::West:
        return Direction::North;
      case Direction::North:
        return Direction::East;
      case Direction::SouthEast:
        return Direction::SouthWest;
      case Direction::SouthWest:
        return Direction::NorthWest;
      case Direction::NorthWest:
        return Direction::NorthEast;
      case Direction::NorthEast:
        return Direction::SouthEast;
      default:
        throw std::invalid_argument("Invalid direction");
    }
  }

  Coords2D Coords2D::step_towards_direction(Direction direction) const {
    switch (direction) {
      case Direction::East:
        return Coords2D{first, second + 1};
      case Direction::SouthEast:
        return Coords2D{first + 1, second + 1};
      case Direction::South:
        return Coords2D{first + 1, second};
      case Direction::SouthWest:
        return Coords2D{first + 1, second - 1};
      case Direction::West:
        return Coords2D{first, second - 1};
      case Direction::NorthWest:
        return Coords2D{first - 1, second - 1};
      case Direction::North:
        return Coords2D{first - 1, second};
      case Direction::NorthEast:
        return Coords2D{first - 1, second + 1};
      default:
        throw std::invalid_argument("Invalid direction");
    }
  }

  cpp_utils::Coords2D step_into_direction(const cpp_utils::Coords2D start_coord,
                                          const cpp_utils::Coords2D direction,
                                          const int num_steps) {
    return start_coord + (direction * num_steps);
  }

  auto normalize_direction(Coords2D const direction) {
    auto const gcd = std::gcd(direction.first, direction.second);
    return Coords2D{direction.first / gcd, direction.second / gcd};
  }

  std::vector<Coords2D> get_all_coords_in_line(Coords2D const start_coord,
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
