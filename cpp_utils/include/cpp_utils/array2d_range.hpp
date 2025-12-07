// Range implementation for Array2D

#pragma once

#include "array2d_iter.hpp"
#include "coords2d.hpp"

#include <type_traits>

namespace cpp_utils {

  template <class C, typename T, bool IsConst>
  class Array2DRange {
    using container_reference = typename std::conditional_t<IsConst, C const&, C&>;
    using container_pointer = typename std::conditional_t<IsConst, C const*, C*>;
    using Iterator = Array2DIterator<C, T, false>;
    using ConstIterator = Array2DIterator<C, T, true>;

    // Signed coordantes are used to represent boundaries in iterators (e.g., -1 for before the
    // first row)
    using Coords = Coords2D<int64_t>;

   public:
    Array2DRange(container_reference array, Coords start_coords, Direction direction, bool flatten)
        : array_(&array), start_coords_(start_coords), direction_(direction), flatten_(flatten) {}

    ConstIterator begin() const {
      return ConstIterator(*array_, start_coords_, direction_, flatten_);
    }

    Iterator begin()
      requires(!IsConst)
    {
      return Iterator(*array_, start_coords_, direction_, flatten_);
    }

    ConstIterator end() const { return ConstIterator(*array_, end_coords(), direction_, flatten_); }

    Iterator end()
      requires(!IsConst)
    {
      return Iterator(*array_, end_coords(), direction_, flatten_);
    }

    Coords start_coords() const { return start_coords_; }
    Coords end_coords() const {
      if (flatten_) {
        return array_->flatten_end_coords(direction_);
      } else {
        return array_->end_coords(start_coords_, direction_);
      }
    }

   private:
    container_pointer array_;
    Coords start_coords_;
    Direction direction_;
    bool flatten_;
  };
}  // namespace cpp_utils
