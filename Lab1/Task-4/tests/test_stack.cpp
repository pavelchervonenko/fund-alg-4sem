#include <gtest/gtest.h>
#include "my_container/stack.hpp"
#include "my_container/deque.hpp"

using my_container::Deque;
using my_container::Stack;

class StackIntTest : public ::testing::Test
{
protected:
    using S = Stack<int, Deque<int>>;
    S empty_;
    S a_{{1, 2, 3}};
    S b_{{1, 2, 4}};
};

TEST_F(StackIntTest, DefaultCtor)
{
    EXPECT_TRUE(empty_.empty());
    EXPECT_EQ(empty_.size(), 0u);
}

TEST_F(StackIntTest, InitListCtor)
{
    EXPECT_FALSE(a_.empty());
    EXPECT_EQ(a_.size(), 3u);
    EXPECT_EQ(a_.top(), 3);
}

TEST(StackCtor, FromContainerLvalue)
{
    Deque<int> d{1, 2};
    Stack<int, Deque<int>> s(d);
    EXPECT_EQ(s.size(), 2u);
    EXPECT_EQ(s.top(), 2);
}

TEST(StackCtor, FromContainerRvalue)
{
    Deque<int> d{5, 7};
    Stack<int, Deque<int>> s(std::move(d));
    EXPECT_EQ(s.size(), 2u);
    EXPECT_EQ(s.top(), 7);
}

TEST_F(StackIntTest, CopyMove)
{
    S cpy(a_);
    EXPECT_EQ(cpy, a_);
    S tmp{{9, 10}};
    S mv(std::move(tmp));
    EXPECT_EQ(mv.size(), 2u);
    EXPECT_EQ(mv.top(), 10);
}

TEST_F(StackIntTest, AssignCopyMove)
{
    S x, y{{1}};
    x = a_;
    EXPECT_EQ(x, a_);
    y = S{{7, 8}};
    EXPECT_EQ(y.size(), 2u);
    EXPECT_EQ(y.top(), 8);
}

TEST_F(StackIntTest, TopOkAndThrows)
{
    EXPECT_EQ(a_.top(), 3);
    EXPECT_THROW(empty_.top(), std::out_of_range);
}

TEST_F(StackIntTest, PushPopLvalueRvalue)
{
    int v = 10;
    empty_.push(v);
    EXPECT_EQ(empty_.top(), 10);
    empty_.push(20);
    EXPECT_EQ(empty_.top(), 20);
    EXPECT_EQ(empty_.size(), 2u);
    empty_.pop();
    EXPECT_EQ(empty_.top(), 10);
    empty_.pop();
    EXPECT_TRUE(empty_.empty());
}

TEST_F(StackIntTest, PopOnEmptyThrows)
{
    EXPECT_THROW(empty_.pop(), std::out_of_range);
}

TEST_F(StackIntTest, Swap)
{
    S x{{1, 2}}, y{{9}};
    swap(x, y);
    EXPECT_EQ(x.size(), 1u);
    EXPECT_EQ(x.top(), 9);
    EXPECT_EQ(y.size(), 2u);
    EXPECT_EQ(y.top(), 2);
}

TEST_F(StackIntTest, EqNeLtLeGtGe)
{
    S x{{1, 2, 3}}, y{{1, 2, 3}}, z{{1, 2, 4}};
    EXPECT_EQ(x, y);
    EXPECT_NE(x, z);
    EXPECT_LT(x, z);
    EXPECT_LE(x, z);
    EXPECT_LE(x, y);
    EXPECT_GT(z, x);
    EXPECT_GE(z, x);
    EXPECT_GE(y, x);
}

#if (__cplusplus >= 202002L) && (defined(__cpp_lib_three_way_comparison) || defined(__cpp_impl_three_way_comparison))
#include <compare>
TEST_F(StackIntTest, ThreeWayIfAvailable)
{
    S x{{1, 2, 3}}, y{{1, 2, 4}};
    auto cmp = (x <=> y);
    EXPECT_EQ(cmp, std::strong_ordering::less);
}
#endif
