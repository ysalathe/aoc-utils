#pragma once

#include <fmt/core.h>

#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace cpp_utils {
  std::optional<std::string> read_given_input_file(int argc, char* argv[]) {
    if (argc < 2) {
      fmt::print(stderr, "Usage: {} <filename>\n", argv[0]);
      return std::nullopt;
    }

    std::ifstream file(argv[1]);
    if (!file) {
      fmt::print(stderr, "Error opening file: {}\n", argv[1]);
      return std::nullopt;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }

  std::vector<std::string> split(std::string const& str, std::string const& delimiter) {
    std::vector<std::string> parts;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
      if (end > start) {
        parts.push_back(str.substr(start, end - start));
      }
      start = end + delimiter.size();
      end = str.find(delimiter, start);
    }
    parts.push_back(str.substr(start, end));
    return parts;
  }
}  // namespace cpp_utils
