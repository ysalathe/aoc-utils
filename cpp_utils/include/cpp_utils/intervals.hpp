// Class to represent and manage intervals of ordered type T.

#pragma once

#include <map>
#include <vector>

namespace cpp_utils {

  template <typename T>
  class Intervals {
    using Interval = std::pair<T, T>;

   public:
    Intervals() = default;
    ~Intervals() = default;
    Intervals(Intervals const&) = default;
    Intervals(Intervals&&) = default;
    Intervals(std::vector<Interval> intervals);

    void insert_interval(Interval interval);
    void insert_interval(T start, T end);

    bool contains(T value) const;

    T total_area() const;

    Intervals& operator=(Intervals const&) = default;
    Intervals& operator=(Intervals&&) = default;

   private:
    // Maps start (inclusive) -> end (inclusive)
    std::map<T, T> intervals_;
  };

}  // namespace cpp_utils

#include "_template_definitions/intervals.tpp"
