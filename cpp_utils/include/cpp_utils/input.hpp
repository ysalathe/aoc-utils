#pragma once

#include <fmt/core.h>

#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace cpp_utils {

  std::optional<std::string> read_input_file_given_by_name(std::string const& filename) {
    std::ifstream file(filename);
    if (!file) {
      fmt::print(stderr, "Error opening file: {}\n", filename);
      return std::nullopt;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }

  std::optional<std::string> read_input_file_given_by_argument(int argc, char* argv[]) {
    if (argc < 2) {
      fmt::print(stderr, "Usage: {} <filename>\n", argv[0]);
      return std::nullopt;
    }

    return read_input_file_given_by_name(argv[1]);
  }

  std::vector<std::string> split_string(std::string const& str, std::string const& delimiter) {
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

  std::vector<std::string> split_string(std::string const& str, char delimiter) {
    return split_string(str, std::string(1, delimiter));
  }

}  // namespace cpp_utils
