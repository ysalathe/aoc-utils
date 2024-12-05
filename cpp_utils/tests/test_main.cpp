// main.cpp
#include "cpp_utils/array2d.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

namespace {

  TEST(Array2DTest, Handles2DArrayWithVector) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
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
        std::vector<std::vector<int>>{std::vector<int>{1, 2, 3}, std::vector<int>{4, 5, 6}});
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
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_upper_left_corner();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratesVertical) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_upper_left_corner(cpp_utils::Direction::Vertical);
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratesDiagonal) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_upper_left_corner(cpp_utils::Direction::Diagonal);
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratesMinorDiagonal) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_upper_right_corner(cpp_utils::Direction::MinorDiagonal);
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratorFrom) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.iterator_from(1, 1);
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, 6);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratesHorizontalFlat) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_upper_left_corner(cpp_utils::Direction::Horizontal, true);
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, 6);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratesVerticalFlat) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_upper_left_corner(cpp_utils::Direction::Vertical, true);
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 6);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, MutableIterator) {
    auto vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> array(2, 3, vec);
    auto it = array.begin_upper_left_corner();
    *it = 10;
    EXPECT_EQ(array(0, 0), 10);
  }

  TEST(Array2DTest, Format) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    std::string expected = "Array2D(2x3)\n1 2 3\n4 5 6\n";
    EXPECT_EQ(fmt::format("{}", array), expected);
  }

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}