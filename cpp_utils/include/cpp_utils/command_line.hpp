#pragma once

#include <fmt/core.h>

#include <fstream>
#include <optional>
#include <sstream>
#include <string>

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
}  // namespace cpp_utils