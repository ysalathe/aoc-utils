// Iterator for Array2D class

#pragma once

#include "coords2d.hpp"
#include "exceptions.hpp"

#include <algorithm>
#include <type_traits>

namespace cpp_utils {

  struct Sentinel {};

  template <class C, typename T, bool IsConst>
  class Array2DIterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = int;
    using pointer = typename std::conditional_t<IsConst,
                                                typename std::vector<T>::const_pointer,
                                                typename std::vector<T>::pointer>;
    using reference = typename std::conditional_t<IsConst,
                                                  typename std::vector<T>::const_reference,
                                                  typename std::vector<T>::reference>;
    using container_reference = typename std::conditional_t<IsConst, C const&, C&>;
    using container_pointer = typename std::conditional_t<IsConst, C const*, C*>;

    // Default constructor
    Array2DIterator() = default;

    Array2DIterator(container_reference array,
                    Coords2D starting_point,
                    Direction direction,
                    bool flatten)
        : array_(&array), coords_(starting_point), direction(direction), flatten(flatten) {
      if (flatten && (direction == Direction::NorthEast || direction == Direction::SouthWest ||
                      direction == Direction::NorthWest || direction == Direction::SouthEast)) {
        throw DiagonalFlattenNotImplemented();
      }
    }

    Direction direction;
    bool flatten;

    auto const& operator*() const {
      assert_not_null();
      return (*array_)(coords_);
    }

    reference operator*()
      requires(!IsConst)
    {
      assert_not_null();
      return (*array_)(coords_);
    }

    pointer operator->() const {
      assert_not_null();
      return &(*array_)(coords_);
    }

    auto coords() const { return coords_; }

    size_t num_neighbors(const T& value, bool diagonal) const {
      // Counts the number neighbors with a given value
      // Args:
      //   value: The value to count as neighbor
      //   diagonal: If true, diagonal neighbors are also counted. If false, only direct neighbors
      //   (N, S, E, W) are counted.
      // Returns:
      //   The number of neighbors with the given value.
      //
      // Note: this function never flattens coordinates.
      // TODO(YSA): Add unit test
      constexpr std::array<Direction, 4> straightDirections =
          std::to_array({Direction::North, Direction::South, Direction::East, Direction::West});
      constexpr std::array<Direction, 4> diagonalDirections = std::to_array(
          {Direction::NorthEast, Direction::NorthWest, Direction::SouthEast, Direction::SouthWest});

      auto countFunction = [&](Direction dir) {
        Coords2D neighborCoords =
            array_->step_coords_towards_direction(coords_, dir, false /* never flatten */);
        if (!array_->is_valid_index(neighborCoords)) {
          return false;
        }
        return (*array_)(neighborCoords) == value;
      };

      size_t count = std::ranges::count_if(straightDirections, countFunction);

      if (diagonal) {
        count += std::ranges::count_if(diagonalDirections, countFunction);
      }
      return count;
    }

    // Pre-increment
    Array2DIterator& operator++() {
      assert_not_null();
      coords_ = array_->step_coords_towards_direction(coords_, direction, flatten);
      return *this;
    }

    // Post-increment
    Array2DIterator operator++(difference_type) {
      assert_not_null();
      Array2DIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    // Pre-decrement
    Array2DIterator& operator--() {
      assert_not_null();
      // Implement the reverse of step_coords_towards_direction
      coords_ =
          array_->step_coords_towards_direction(coords_, reverse_direction(direction), flatten);
      return *this;
    }

    // Post-decrement
    Array2DIterator operator--(difference_type) {
      assert_not_null();
      Array2DIterator tmp = *this;
      --(*this);
      return tmp;
    }

    Array2DIterator& operator+(difference_type n) {
      assert_not_null();
      for (difference_type k = 0; k < n; ++k) {
        ++(*this);
      }
      return *this;
    }

    Array2DIterator& operator-(difference_type n) {
      assert_not_null();
      for (difference_type k = 0; k < n; ++k) {
        --(*this);
      }
      return *this;
    }

    bool operator==(Array2DIterator const& other) const { return coords_ == other.coords_; }
    bool operator==(Sentinel const&) const {
      assert_not_null();
      return !array_->is_valid_index(coords_);
    }

   private:
    void assert_not_null() const {
      if (array_ == nullptr) {
        throw std::logic_error("Iterator is not initialized.");
      }
    }

    container_pointer array_ = NULL;
    Coords2D coords_;
  };

}  // namespace cpp_utils
