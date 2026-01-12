#pragma once

#include <cpp_utils/search.hpp>

namespace cpp_utils {

  template <typename T, bool FindAll, bool FindAllDistinct, class Hash>
  std::conditional_t<
      FindAll,
      std::conditional_t<FindAllDistinct, std::unordered_set<T, Hash>, std::vector<T>>,
      T>
  depthFirstSearch(T start,
                   std::function<std::vector<T>(T)> const& visitAndGetSuccessors,
                   std::function<bool(T)> const& isGoal) {
    std::vector<T> stack = {start};
    std::conditional_t<
        FindAll, std::conditional_t<FindAllDistinct, std::unordered_set<T, Hash>, std::vector<T>>,
        T>
        result;
    if constexpr (!FindAll) {
      result = start;
    }
    while (!stack.empty()) {
      auto current = stack.back();
      stack.pop_back();
      if (isGoal(current)) {
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
      for (auto const& successor : visitAndGetSuccessors(current)) {
        stack.push_back(successor);
      }
    }
    return result;
  }

  template <typename T, bool FindAll, bool FindAllDistinct, class Hash>
  std::conditional_t<
      FindAll,
      std::conditional_t<FindAllDistinct, std::unordered_set<T, Hash>, std::vector<T>>,
      T>
  breadthFirstSearch(T start,
                     std::function<std::vector<T>(T)> const& visitAndGetSuccessors,
                     std::function<bool(T)> const& isGoal) {
    std::vector<T> queue = {start};
    std::conditional_t<
        FindAll, std::conditional_t<FindAllDistinct, std::unordered_set<T, Hash>, std::vector<T>>,
        T>
        result;
    if constexpr (!FindAll) {
      result = start;
    }
    while (!queue.empty()) {
      auto current = queue.front();
      queue.erase(queue.begin());
      if (isGoal(current)) {
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
      for (auto const& successor : visitAndGetSuccessors(current)) {
        queue.push_back(successor);
      }
    }
    return result;
  }
}  // namespace cpp_utils
