
# Add the test executable
add_executable(test_exec test_main.cpp)
gtest_discover_tests(test_exec)

# Link the Google Test library and pthread
target_link_libraries(test_exec ${GTEST_LIBRARIES} pthread cpp_utils)
