// main.cpp
#include "cpp_utils/array2d.hpp"
#include "fmt/format.h"

#include <gtest/gtest.h>

namespace {

  TEST(Array2DTest, Handles2DArrayWithVector) {
    cpp_utils::Array2D<int> const array(2, 3, std::vector<int>{1, 2, 3, 4, 5, 6});
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    EXPECT_EQ(array(0, 0), 1);
    EXPECT_EQ(array(0, 1), 2);
    EXPECT_EQ(array(0, 2), 3);
    EXPECT_EQ(array(1, 0), 4);
    EXPECT_EQ(array(1, 1), 5);
    EXPECT_EQ(array(1, 2), 6);
  }

  TEST(Array2DTest, Handles2DArrayWithNestedVector) {
    cpp_utils::Array2D<int> const array(
        2, 3, std::vector<std::vector<int>>{std::vector<int>{1, 2, 3}, std::vector<int>{4, 5, 6}});
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    EXPECT_EQ(array(0, 0), 1);
    EXPECT_EQ(array(0, 1), 2);
    EXPECT_EQ(array(0, 2), 3);
    EXPECT_EQ(array(1, 0), 4);
    EXPECT_EQ(array(1, 1), 5);
    EXPECT_EQ(array(1, 2), 6);
  }

  TEST(Array2DTest, Handles2DArrayWithDefaultValues) {
    cpp_utils::Array2D<int> const array(2, 3, 0);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    EXPECT_EQ(array(0, 0), 0);
    EXPECT_EQ(array(0, 1), 0);
    EXPECT_EQ(array(0, 2), 0);
    EXPECT_EQ(array(1, 0), 0);
    EXPECT_EQ(array(1, 1), 0);
    EXPECT_EQ(array(1, 2), 0);
  }

  TEST(Array2DTest, IteratesHorizontal) {
    cpp_utils::Array2D<int> const array(2, 3, {1, 2, 3, 4, 5, 6});
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.upper_left_corner();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, std::nullopt);
  }

  TEST(Array2DTest, IteratesVertical) {
    cpp_utils::Array2D<int> const array(2, 3, {1, 2, 3, 4, 5, 6});
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.upper_left_corner<cpp_utils::Array2D<int>::Direction::Vertical>();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, std::nullopt);
  }

  TEST(Array2DTest, IteratesDiagonal) {
    cpp_utils::Array2D<int> const array(2, 3, {1, 2, 3, 4, 5, 6});
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.upper_left_corner<cpp_utils::Array2D<int>::Direction::Diagonal>();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, std::nullopt);
  }

  TEST(Array2DTest, IteratesMinorDiagonal) {
    cpp_utils::Array2D<int> const array(2, 3, {1, 2, 3, 4, 5, 6});
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.upper_right_corner<cpp_utils::Array2D<int>::Direction::MinorDiagonal>();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, std::nullopt);
  }

  TEST(Array2DTest, PrintWithFmt) {
    cpp_utils::Array2D<int> const array(2, 3, {1, 2, 3, 4, 5, 6});
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    fmt::print("{}", array);
  }

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}