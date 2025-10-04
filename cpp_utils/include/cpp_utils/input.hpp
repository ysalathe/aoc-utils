#pragma once

#include <fmt/core.h>

#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace cpp_utils {

  std::optional<std::string> readInputFileGivenByName(std::string const& filename);

  std::optional<std::string> readInputFileGivenByArgument(int argc, char* argv[]);

  std::vector<std::string_view> splitString(std::string_view const& str,
                                            std::string_view const& delimiter);

  std::vector<std::string_view> splitString(std::string_view const& str, char delimiter);

}  // namespace cpp_utils
