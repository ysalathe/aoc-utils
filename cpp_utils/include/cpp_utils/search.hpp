#pragma once
#include <functional>
#include <type_traits>
#include <unordered_set>
#include <vector>
namespace cpp_utils {

  template <typename T, bool FindAll = false, bool FindAllDistinct = true>
  std::conditional_t<FindAll,
                     std::conditional_t<FindAllDistinct, std::unordered_set<T>, std::vector<T>>,
                     T>
  depth_first_search(T start,
                     std::function<std::vector<T>(T)> const& get_successors,
                     std::function<bool(T)> const& is_goal) {
    std::vector<T> stack = {start};
    std::conditional_t<
        FindAll, std::conditional_t<FindAllDistinct, std::unordered_set<T>, std::vector<T>>, T>
        result;
    if constexpr (!FindAll) {
      result = start;
    }
    while (!stack.empty()) {
      auto current = stack.back();
      stack.pop_back();
      if (is_goal(current)) {
        if constexpr (FindAll) {
          if constexpr (FindAllDistinct) {
            result.insert(current);
          } else {
            result.push_back(current);
          }
        } else {
          return current;
        }
      }
      for (auto const& successor : get_successors(current)) {
        stack.push_back(successor);
      }
    }
    return result;
  }

  template <typename T, bool FindAll = false, bool FindAllDistinct = true>
  std::conditional_t<FindAll,
                     std::conditional_t<FindAllDistinct, std::unordered_set<T>, std::vector<T>>,
                     T>
  breadth_first_search(T start,
                       std::function<std::vector<T>(T)> const& get_successors,
                       std::function<bool(T)> const& is_goal) {
    std::vector<T> queue = {start};
    std::conditional_t<
        FindAll, std::conditional_t<FindAllDistinct, std::unordered_set<T>, std::vector<T>>, T>
        result;
    if constexpr (!FindAll) {
      result = start;
    }
    while (!queue.empty()) {
      auto current = queue.front();
      queue.erase(queue.begin());
      if (is_goal(current)) {
        if constexpr (FindAll) {
          if constexpr (FindAllDistinct) {
            result.insert(current);
          } else {
            result.push_back(current);
          }
        } else {
          return current;
        }
      }
      for (auto const& successor : get_successors(current)) {
        queue.push_back(successor);
      }
    }
    return result;
  }
}  // namespace cpp_utils
