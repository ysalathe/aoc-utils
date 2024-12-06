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

  TEST(Array2DTest, IteratesEast) {
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

  TEST(Array2DTest, IteratesSouthEast) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_upper_left_corner(cpp_utils::Direction::SouthEast);
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratesSouth) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_upper_left_corner(cpp_utils::Direction::South);
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratesSouthWest) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_upper_right_corner(cpp_utils::Direction::SouthWest);
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratesWest) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_upper_right_corner(cpp_utils::Direction::West);
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratesNorthWest) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_lower_right_corner(cpp_utils::Direction::NorthWest);
    EXPECT_EQ(*it, 6);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratesNorth) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_lower_left_corner(cpp_utils::Direction::North);
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratesNorthEast) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_lower_left_corner(cpp_utils::Direction::NorthEast);
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 2);
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

  TEST(Array2DTest, IteratesEastFlat) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_upper_left_corner(cpp_utils::Direction::East, true);
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

  TEST(Array2DTest, IteratesSouthFlat) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_upper_left_corner(cpp_utils::Direction::South, true);
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

  TEST(Array2DTest, IteratesWestFlat) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_lower_right_corner(cpp_utils::Direction::West, true);
    EXPECT_EQ(*it, 6);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(it, array.end());
  }

  TEST(Array2DTest, IteratesNorthFlat) {
    auto const vec = std::vector<int>{1, 2, 3, 4, 5, 6};
    cpp_utils::Array2D<int> const array(2, 3, vec);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    auto it = array.begin_lower_right_corner(cpp_utils::Direction::North, true);
    EXPECT_EQ(*it, 6);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 1);
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

  TEST(Array2DTest, FormatEmpty) {
    cpp_utils::Array2D<int> const array(0, 0);
    std::string expected = "Array2D(0x0)\n";
    EXPECT_EQ(fmt::format("{}", array), expected);
  }

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}