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
                     std::function<bool(T)> const& is_goal);

  template <typename T, bool FindAll = false, bool FindAllDistinct = true>
  std::conditional_t<FindAll,
                     std::conditional_t<FindAllDistinct, std::unordered_set<T>, std::vector<T>>,
                     T>
  breadth_first_search(T start,
                       std::function<std::vector<T>(T)> const& get_successors,
                       std::function<bool(T)> const& is_goal);
}  // namespace cpp_utils

#include "_template_definitions/search.tpp"
