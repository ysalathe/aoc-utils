
# Set the project name
project(cpp_utils)

# Add the library
add_library(${PROJECT_NAME} INTERFACE)

# Specify the include directories
target_include_directories(${PROJECT_NAME} INTERFACE include)

# Add the tests subdirectory
add_subdirectory(tests)

target_link_libraries(${PROJECT_NAME} INTERFACE fmt::fmt)
