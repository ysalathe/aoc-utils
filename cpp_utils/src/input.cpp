#include <cpp_utils/input.hpp>

#include <stdexcept>

namespace cpp_utils {

  std::string readInputFileGivenByName(std::string const& filename) {
    std::ifstream file(filename);
    if (!file) {
      throw std::runtime_error(fmt::format("Error opening file: {}", filename));
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }

  std::string readInputFileGivenByArgument(const int argc, char* argv[]) {
    if (argc < 2) {
      throw std::invalid_argument(
          fmt::format("Not enough arguments provided. Usage: {} <filename>\n", argv[0]));
    }
    std::string input = readInputFileGivenByName(argv[1]);
    return input;
  }

  std::vector<std::string_view> splitString(std::string_view const& str,
                                            std::string_view const& delimiter) {
    std::vector<std::string_view> parts;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string_view::npos) {
      if (end > start) {
        parts.push_back(str.substr(start, end - start));
      }
      start = end + delimiter.size();
      end = str.find(delimiter, start);
    }
    parts.push_back(str.substr(start, end));
    return parts;
  }

  std::vector<std::string_view> splitString(std::string_view const& str, char delimiter) {
    return splitString(str, std::string(1, delimiter));
  }
}  // namespace cpp_utils
