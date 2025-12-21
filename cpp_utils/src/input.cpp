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

  std::string_view ltrim(std::string_view sv) {
    // Trim whitespaces from start of sv
    auto it =
        std::find_if(sv.begin(), sv.end(), [](unsigned char ch) { return !std::isspace(ch); });
    return std::string_view(it, sv.end());
  }

  std::string_view rtrim(std::string_view sv) {
    // Trim whitespaces from end of sv
    auto it =
        std::find_if(sv.rbegin(), sv.rend(), [](unsigned char ch) { return !std::isspace(ch); });
    return std::string_view(sv.begin(), it.base());
  }

  std::string_view trim(std::string_view sv) {
    // Trim from start and end of sv
    return rtrim(ltrim(sv));
  }

}  // namespace cpp_utils
