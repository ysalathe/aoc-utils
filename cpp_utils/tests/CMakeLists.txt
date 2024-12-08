
# Add the test executable
add_executable(test_array2d test_array2d.cpp)
gtest_discover_tests(test_array2d)

# Link the Google Test library and pthread
target_link_libraries(test_array2d ${GTEST_LIBRARIES} gtest_main pthread cpp_utils)