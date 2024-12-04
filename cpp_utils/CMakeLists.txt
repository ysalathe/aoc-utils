cmake_minimum_required(VERSION 3.10)
# Set the project name
project(cpp_utils)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Add the library
add_library(${PROJECT_NAME} INTERFACE)

# Specify the include directories
target_include_directories(${PROJECT_NAME} INTERFACE include)

# Add the tests subdirectory
add_subdirectory(tests)

# External libraries
include(FetchContent)
include(CMakePrintHelpers)

set(FETCHCONTENT_QUIET FALSE)

# fmt
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        10.1.1
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
)
option(FMT_SYSTEM_HEADERS ON)
FetchContent_MakeAvailable(fmt)

target_link_libraries(${PROJECT_NAME} INTERFACE fmt::fmt)