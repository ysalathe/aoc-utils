cmake_minimum_required(VERSION 3.10)

# Set the project name
project(cpp_utils)

# Add the library
add_library(cpp_utils INTERFACE)

# Specify the include directories
target_include_directories(cpp_utils INTERFACE include)

# Add the main executable
add_executable(main_exec tests/main.cpp)

# Link the library to the executable
target_link_libraries(main_exec cpp_utils)