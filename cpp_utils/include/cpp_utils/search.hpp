#pragma once
#include <functional>
#include <vector>

namespace cpp_utils {
  template <typename T>
  T depth_first_search(T start,
                       std::function<std::vector<T>(T)> const& get_successors,
                       std::function<bool(T)> const& is_goal) {
    std::vector<T> stack = {start};
    while (!stack.empty()) {
      auto current = stack.back();
      stack.pop_back();
      if (is_goal(current)) {
        return current;
      }
      for (auto const& successor : get_successors(current)) {
        stack.push_back(successor);
      }
    }
    return start;
  }
}  // namespace cpp_utils
