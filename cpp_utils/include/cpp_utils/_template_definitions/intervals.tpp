// Implementation of Intervals class template.

#pragma once

#include <cpp_utils/intervals.hpp>
#include <fmt/format.h>

#include <algorithm>
#include <stdexcept>

namespace cpp_utils {

  template <typename T>
  Intervals<T>::Intervals(std::vector<Interval> intervals) {
    for (auto const& interval : intervals) {
      insert_interval(interval.first, interval.second);
    }
  }

  template <typename T>
  void Intervals<T>::insert_interval(Intervals<T>::Interval interval) {
    insert_interval(interval.first, interval.second);
  }

  template <typename T>
  void Intervals<T>::insert_interval(T start, T end) {
    // Insert a new interval [start, end] (both inclusive)
    //
    // Replaces exsisting overlapping intervals.
    //
    // Args:
    //   start: start of the interval
    //   end: end of the interval
    // Note: the exisitng intervals are assumed to be non-overlapping.

    if (start > end) {
      throw std::invalid_argument(
          fmt::format("Interval start {} must be less than or equal to end {}.", start, end));
    }

    // Find the first interval whose start (key) is greater than the new interval's end
    // i.e. it->first > end
    auto it = intervals_.upper_bound(end);

    // This interval certainly doesn't overlap with the new interval
    // so go back one to check for overlapping intervals
    if (it != intervals_.begin()) {
      --it;
    }

    while (it != intervals_.end() && it->first <= end && it->second >= start) {
      // Merge overlapping intervals
      start = std::min(start, it->first);
      end = std::max(end, it->second);
      // Erase the overlapping interval
      intervals_.erase(it);
      // Cannot reuse the iterator after erasing, so find the next interval again
      it = intervals_.upper_bound(end);
      if (it != intervals_.begin()) {
        --it;
      }
    }
    intervals_.emplace(start, end);
  }

  template <typename T>
  bool Intervals<T>::contains(T value) const {
    // Check if the value is contained in any of the intervals

    // Find the first interval whose start (key) is greater than the value
    // i.e. it->first > value
    auto it = intervals_.upper_bound(value);

    // If this is the first interval, the value is not contained in any interval
    if (it == intervals_.begin()) {
      return false;
    }

    // Go back one to check if the value is in the previous interval
    --it;
    return it->first <= value && it->second >= value;
  }

  template <typename T>
  T Intervals<T>::total_area() const {
    // Calculate the total area covered by all intervals
    return std::ranges::fold_left(intervals_, static_cast<T>(0), [](T acc, auto const& interval) {
      return acc + (interval.second - interval.first + 1);
    });
  }

}  // namespace cpp_utils
