#pragma once

#include <functional>
#include <type_traits>
#include <unordered_set>
#include <vector>
namespace cpp_utils {

  template <typename T,
            bool FindAll = false,
            bool FindAllDistinct = true,
            class Hash = std::hash<T>>
  std::conditional_t<
      FindAll,
      std::conditional_t<FindAllDistinct, std::unordered_set<T, Hash>, std::vector<T>>,
      T>
  depthFirstSearch(T start,
                   std::function<std::vector<T>(T)> const& visitAndGetSuccessors,
                   std::function<bool(T)> const& isGoal);

  template <typename T,
            bool FindAll = false,
            bool FindAllDistinct = true,
            class Hash = std::hash<T>>
  std::conditional_t<
      FindAll,
      std::conditional_t<FindAllDistinct, std::unordered_set<T, Hash>, std::vector<T>>,
      T>
  breadthFirstSearch(T start,
                     std::function<std::vector<T>(T)> const& visitAndGetSuccessors,
                     std::function<bool(T)> const& isGoal);
}  // namespace cpp_utils

#include "_template_definitions/search.tpp"
