
# Set the project name
project(cpp_utils)

# Add the library
add_library(${PROJECT_NAME} INTERFACE)

# Specify the include directories
target_include_directories(${PROJECT_NAME} INTERFACE include)

# Add soures to target
target_sources(${PROJECT_NAME}
PUBLIC
src/array2d.cpp
src/coords2d.cpp
src/input.cpp)

# Add the tests subdirectory
add_subdirectory(tests)

target_link_libraries(${PROJECT_NAME} INTERFACE fmt::fmt)
