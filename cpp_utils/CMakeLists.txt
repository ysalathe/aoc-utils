cmake_minimum_required(VERSION 3.10)
# Set the project name
project(cpp_utils)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the library
add_library(${PROJECT_NAME} INTERFACE)

# Specify the include directories
target_include_directories(${PROJECT_NAME} INTERFACE include)

# Add the main executable
add_executable(main_exec tests/main.cpp)

# Link the library to the executable
target_link_libraries(main_exec ${PROJECT_NAME})
