# tests/CMakeLists.txt
enable_testing()

# Add the Google Test library
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add the test executable
add_executable(test_exec test_main.cpp)

# Link the Google Test library and pthread
target_link_libraries(test_exec ${GTEST_LIBRARIES} pthread cpp_utils)

# Add a test
add_test(NAME test_exec COMMAND test_exec)