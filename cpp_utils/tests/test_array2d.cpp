// main.cpp
#include "cpp_utils/array2d.hpp"
#include "cpp_utils/array2d_formatter.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <ranges>
#include <utility>

namespace {

  // Factory functions to create different derivatives of Array2DBase
  // See https://github.com/google/googletest/blob/main/googletest/samples/sample6_unittest.cc
  template <class C>
  cpp_utils::Array2DBase<int>* CreateArray2DBase();

  auto const default_test_vec = std::vector<int>{1, 2, 3, 4, 5, 6};

  template <>
  cpp_utils::Array2DBase<int>* CreateArray2DBase<cpp_utils::Array2D<int>>() {
    return new cpp_utils::Array2D<int>(2, 3, default_test_vec);
  }

  template <>
  cpp_utils::Array2DBase<int>* CreateArray2DBase<cpp_utils::SparseArray2D<int>>() {
    return new cpp_utils::SparseArray2D<int>(2, 3, default_test_vec, 0);
  }

  template <class C>
  cpp_utils::Array2DBase<int>* CreateEmptyArray2DBase();

  template <>
  cpp_utils::Array2DBase<int>* CreateEmptyArray2DBase<cpp_utils::Array2D<int>>() {
    return new cpp_utils::Array2D<int>(0, 0);
  }

  template <>
  cpp_utils::Array2DBase<int>* CreateEmptyArray2DBase<cpp_utils::SparseArray2D<int>>() {
    return new cpp_utils::SparseArray2D<int>(0, 0, 0);
  }

  // Then we define a test fixture class template.
  template <class C>
  class Array2DBaseTest : public testing::Test {
   protected:
    // The creator calls the factory function to create a prime table
    // implemented by T.
    Array2DBaseTest() : array_(CreateArray2DBase<C>()), empty_array_(CreateEmptyArray2DBase<C>()) {}

    ~Array2DBaseTest() override { delete array_; }

    // Test
    cpp_utils::Array2DBase<int>* const array_;
    cpp_utils::Array2DBase<int>* const empty_array_;
  };

  using testing::Types;
  typedef Types<cpp_utils::Array2D<int>, cpp_utils::SparseArray2D<int>> Implementations;

  TYPED_TEST_SUITE(Array2DBaseTest, Implementations);

  TYPED_TEST(Array2DBaseTest, IteratesEast) {
    auto range = this->array_->range_from(this->array_->upper_left_corner());
    auto it = range.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, range.end());
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesPartialRange) {
    auto range = this->array_->range_from({0, 1});
    auto it = range.begin();
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, range.end());
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesPartialRangeEndMinusOne) {
    auto range = this->array_->range_from(this->array_->upper_left_corner());
    auto it = range.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(it, range.end() - 1);
  }

  TYPED_TEST(Array2DBaseTest, IteratesSouthEast) {
    auto range = this->array_->range_from(this->array_->upper_left_corner(),
                                          cpp_utils::Direction::SouthEast);
    auto it = range.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(it, range.end());
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesSouth) {
    auto range =
        this->array_->range_from(this->array_->upper_left_corner(), cpp_utils::Direction::South);
    auto it = range.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(it, range.end());
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesSouthWest) {
    auto range = this->array_->range_from(this->array_->upper_right_corner(),
                                          cpp_utils::Direction::SouthWest);
    auto it = range.begin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(it, range.end());
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesWest) {
    auto range =
        this->array_->range_from(this->array_->upper_right_corner(), cpp_utils::Direction::West);
    auto it = range.begin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(it, range.end());
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesNorthWest) {
    auto range = this->array_->range_from(this->array_->lower_right_corner(),
                                          cpp_utils::Direction::NorthWest);
    auto it = range.begin();
    EXPECT_EQ(*it, 6);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(it, range.end());
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesNorth) {
    auto range =
        this->array_->range_from(this->array_->lower_left_corner(), cpp_utils::Direction::North);
    auto it = range.begin();
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(it, range.end());
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesNorthEast) {
    auto range = this->array_->range_from(this->array_->lower_left_corner(),
                                          cpp_utils::Direction::NorthEast);
    auto it = range.begin();
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(it, range.end());
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesDefaultFlatten) {
    auto it = this->array_->begin();
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
    EXPECT_EQ(it, this->array_->end());
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesSouthFlat) {
    constexpr auto dir = cpp_utils::Direction::South;
    auto it = this->array_->begin(dir);
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
    EXPECT_EQ(it, this->array_->end(dir));
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesWestFlat) {
    constexpr auto dir = cpp_utils::Direction::West;
    auto it = this->array_->begin(dir);
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
    EXPECT_EQ(it, this->array_->end(dir));
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesNorthFlat) {
    constexpr auto dir = cpp_utils::Direction::North;
    auto it = this->array_->begin(dir);
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
    EXPECT_EQ(it, this->array_->end(dir));
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, IteratesBackwards) {
    auto it = this->array_->end();
    --it;
    EXPECT_EQ(*it, 6);
    --it;
    EXPECT_EQ(*it, 5);
    --it;
    EXPECT_EQ(*it, 4);
    --it;
    EXPECT_EQ(*it, 3);
    --it;
    EXPECT_EQ(*it, 2);
    --it;
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(it, this->array_->begin());
    --it;
    EXPECT_EQ(it, cpp_utils::Sentinel());
  }

  TYPED_TEST(Array2DBaseTest, MutableIterator) {
    auto it = this->array_->begin();
    *it = 10;
    EXPECT_EQ((*this->array_)(0, 0), 10);
  }

  TYPED_TEST(Array2DBaseTest, Format) {
    std::string expected = "Array2DBase(2x3)\n1 2 3\n4 5 6\n";
    EXPECT_EQ(fmt::format("{}", *(this->array_)), expected);
  }

  TYPED_TEST(Array2DBaseTest, FormatEmpty) {
    std::string expected = "Array2DBase(0x0)\n";
    EXPECT_EQ(fmt::format("{}", *(this->empty_array_)), expected);
  }

  TYPED_TEST(Array2DBaseTest, StdFindWorks) {
    auto it = std::ranges::find(std::as_const(*(this->array_)), 3);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(it.coords(), cpp_utils::Coords2D(0, 2));
  }

  // Specialized tests for Array2D
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
    // 0 0 0
    // 0 0 0
    EXPECT_EQ(array(0, 0), 0);
    EXPECT_EQ(array(0, 1), 0);
    EXPECT_EQ(array(0, 2), 0);
    EXPECT_EQ(array(1, 0), 0);
    EXPECT_EQ(array(1, 1), 0);
    EXPECT_EQ(array(1, 2), 0);
  }

  // Specialized tests for SparseArray2D
  TEST(SparseArray2DTest, Handles2DArrayWithNestedVector) {
    cpp_utils::SparseArray2D<int> const array(
        std::vector<std::vector<int>>{std::vector<int>{1, 2, 3}, std::vector<int>{4, 5, 6}}, 0);
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

  TEST(SparseArray2DTest, Handles2DArrayOnlyEmpty) {
    cpp_utils::SparseArray2D<int> const array(2, 3, 0);
    EXPECT_EQ(array.empty_element(), 0);
    // The array looks like this:
    // 0 0 0
    // 0 0 0
    EXPECT_EQ(array(0, 0), 0);
    EXPECT_EQ(array(0, 1), 0);
    EXPECT_EQ(array(0, 2), 0);
    EXPECT_EQ(array(1, 0), 0);
    EXPECT_EQ(array(1, 1), 0);
    EXPECT_EQ(array(1, 2), 0);
    EXPECT_EQ(array.size(), 0);
  }

  TEST(SparseArray2DTest, Handles2DArrayOnlyOneNonEmpty) {
    cpp_utils::SparseArray2D<int> array(2, 3, 0);
    array(1, 1) = 1;
    EXPECT_EQ(array.empty_element(), 0);
    // The array looks like this:
    // 0 0 0
    // 0 1 0
    EXPECT_EQ(std::as_const(array)(0, 0), 0);
    EXPECT_EQ(std::as_const(array)(0, 1), 0);
    EXPECT_EQ(std::as_const(array)(0, 2), 0);
    EXPECT_EQ(std::as_const(array)(1, 0), 0);
    EXPECT_EQ(std::as_const(array)(1, 1), 1);
    EXPECT_EQ(std::as_const(array)(1, 2), 0);
    EXPECT_EQ(array.size(), 1);
  }

  TEST(SparseArray2DTest, CleanupWorksForEmptyArray) {
    cpp_utils::SparseArray2D<int> array(2, 3, 0);
    array(1, 1) = 1;
    EXPECT_EQ(array.empty_element(), 0);
    // The array looks like this:
    // 0 0 0
    // 0 1 0
    EXPECT_EQ(array(0, 0), 0);
    EXPECT_EQ(array(0, 1), 0);
    EXPECT_EQ(array(0, 2), 0);
    EXPECT_EQ(array(1, 0), 0);
    EXPECT_EQ(array(1, 1), 1);
    EXPECT_EQ(array(1, 2), 0);
    array.cleanup();
    EXPECT_EQ(array.size(), 1);
  }

  TEST(SparseArray2DTest, CleanupWorks) {
    cpp_utils::SparseArray2D<int> array(2, 3, 0);
    array(1, 1) = 1;
    EXPECT_EQ(array.empty_element(), 0);
    // The array looks like this:
    // 0 0 0
    // 0 1 0
    EXPECT_EQ(array(0, 0), 0);
    EXPECT_EQ(array(0, 1), 0);
    EXPECT_EQ(array(0, 2), 0);
    EXPECT_EQ(array(1, 0), 0);
    EXPECT_EQ(array(1, 1), 1);
    EXPECT_EQ(array(1, 2), 0);
    array.cleanup();
    EXPECT_EQ(array.size(), 1);
  }

  TEST(SparseArray2DTest, CleanupWorksInConstructor) {
    auto const vec = std::vector<int>{3, 3, 3, 3, 3, 3};
    cpp_utils::SparseArray2D<int> array(2, 3, vec, 3);
    EXPECT_EQ(array.empty_element(), 3);
    // The array looks like this:
    // 3 3 3
    // 3 3 3

    // since all elements are the empty element (3), we expect the size to be 0
    EXPECT_EQ(array.size(), 0);
    EXPECT_EQ(array(0, 0), 3);
    EXPECT_EQ(array(0, 1), 3);
    EXPECT_EQ(array(0, 2), 3);
    EXPECT_EQ(array(1, 0), 3);
    EXPECT_EQ(array(1, 1), 3);
    EXPECT_EQ(array(1, 2), 3);
  }

  // Builder tests
  TEST(Array2DBuilderTest, CreateArray2DFromString) {
    auto const input = std::string("1 2 3\n4 5 6\n");
    auto const array = cpp_utils::Array2DBuilder<int>::create_from_string(input);
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

  // Builder tests
  TEST(Array2DBuilderTest, CreateSparseArray2DFromString) {
    auto const input = std::string("1 2 3\n4 5 6\n");
    auto const array = cpp_utils::Array2DBuilder<int>::create_sparse_from_string(input, 0);
    // The array looks like this:
    // 1 2 3
    // 4 5 6
    EXPECT_EQ(array(0, 0), 1);
    EXPECT_EQ(array(0, 1), 2);
    EXPECT_EQ(array(0, 2), 3);
    EXPECT_EQ(array(1, 0), 4);
    EXPECT_EQ(array(1, 1), 5);
    EXPECT_EQ(array(1, 2), 6);
    EXPECT_EQ(array.size(), 6);
  }

  TEST(Array2DBuilderTest, CreateSparseArray2DFromStringEmptyColumnSeparator) {
    auto const input = std::string("123\n456\n");
    auto const array =
        cpp_utils::Array2DBuilder<int>::create_sparse_from_string(input, 0, "\n", "");
    // The array looks like this:
    // 123
    // 456
    EXPECT_EQ(array(0, 0), 1);
    EXPECT_EQ(array(0, 1), 2);
    EXPECT_EQ(array(0, 2), 3);
    EXPECT_EQ(array(1, 0), 4);
    EXPECT_EQ(array(1, 1), 5);
    EXPECT_EQ(array(1, 2), 6);
    EXPECT_EQ(array.size(), 6);
  }

  TEST(Array2DBuilderTest, CreateArray2DChar) {
    auto const input = std::string("a b c\nd e f\n");
    auto const array = cpp_utils::Array2DBuilder<char>::create_from_string(input);
    // The array looks like this:
    // a b c
    // d e f
    EXPECT_EQ(array(0, 0), 'a');
    EXPECT_EQ(array(0, 1), 'b');
    EXPECT_EQ(array(0, 2), 'c');
    EXPECT_EQ(array(1, 0), 'd');
    EXPECT_EQ(array(1, 1), 'e');
    EXPECT_EQ(array(1, 2), 'f');
  }

  TEST(Array2DBuilderTest, CreateArray2DCharEmptyColumnSeparator) {
    auto const input = std::string("abc\ndef\n");
    auto const array = cpp_utils::Array2DBuilder<char>::create_from_string(input, "\n", "");
    // The array looks like this:
    // abc
    // def
    EXPECT_EQ(array(0, 0), 'a');
    EXPECT_EQ(array(0, 1), 'b');
    EXPECT_EQ(array(0, 2), 'c');
    EXPECT_EQ(array(1, 0), 'd');
    EXPECT_EQ(array(1, 1), 'e');
    EXPECT_EQ(array(1, 2), 'f');
  }

}  // namespace
