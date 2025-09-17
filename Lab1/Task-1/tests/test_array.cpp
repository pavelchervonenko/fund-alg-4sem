#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "my_container/array.hpp"

using my_container::Array;

class ArrayIntTest : public ::testing::Test
{
protected:
    enum
    {
        N = 3
    };
    Array<int, N> make123() const
    {
        return Array<int, N>{1, 2, 3};
    }
    Array<int, N> make132() const
    {
        return Array<int, N>{1, 3, 2};
    }
};

class ArrayStringTest : public ::testing::Test
{
protected:
    enum
    {
        N = 3
    };
    Array<std::string, N> makeABC() const
    {
        return Array<std::string, N>{"a", "b", "c"};
    }

    Array<std::string, N> makeACB() const
    {
        return Array<std::string, N>{"a", "c", "b"};
    }
};

// конструкторы

TEST_F(ArrayIntTest, DefaultCtor_ValueInitialized)
{
    Array<int, N> a;
    EXPECT_EQ(a.size(), N);
    for (auto x : a)
    {
        EXPECT_EQ(x, 0);
    }
}

TEST_F(ArrayIntTest, InitListCtor_ExactSize)
{
    auto a = make123();
    EXPECT_EQ(a.size(), N);
    EXPECT_EQ(a.front(), 1);
    EXPECT_EQ(a.back(), 3);
}

TEST_F(ArrayIntTest, InitListCtor_WrongSizeThrows)
{
    EXPECT_THROW((Array<int, N>{1, 2}), std::length_error);
    EXPECT_THROW((Array<int, N>{1, 2, 3, 4}), std::length_error);
}

TEST_F(ArrayIntTest, CopyCtor)
{
    auto a = make123();
    Array<int, N> b{a};
    EXPECT_EQ(a, b);
}

TEST_F(ArrayIntTest, MoveCtor)
{
    auto a = make123();
    Array<int, N> b{std::move(a)};
    EXPECT_EQ(b.size(), N);
}

// присваивания

TEST_F(ArrayIntTest, CopyAssign)
{
    auto a = make123();
    Array<int, N> b{};
    b = a;
    EXPECT_EQ(a, b);
}

TEST_F(ArrayIntTest, MoveAssign)
{
    auto a = make123();
    Array<int, N> b;
    b = std::move(a);
    EXPECT_EQ(b.size(), N);
}

// доступ

TEST_F(ArrayIntTest, At_OkAndThrows)
{
    auto a = make123();
    EXPECT_NO_THROW((void)a.at(0));
    EXPECT_NO_THROW((void)a.at(N - 1));
    EXPECT_THROW((void)a.at(N), std::out_of_range);

    const auto &ca = a;
    EXPECT_NO_THROW((void)ca.at(0));
    EXPECT_THROW((void)ca.at(N), std::out_of_range);
}

TEST_F(ArrayIntTest, Subscript)
{
    auto a = make123();
    a[0] = 7;
    EXPECT_EQ(a.front(), 7);
}

TEST_F(ArrayIntTest, FrontBackData)
{
    auto a = make123();
    EXPECT_EQ(*a.data(), a.front());
    EXPECT_EQ(*(a.data() + (a.size() - 1)), a.back());
}

// итераторы

TEST_F(ArrayIntTest, ForwardIterators)
{
    auto a = make123();
    std::vector<int> v;
    for (auto it = a.begin(); it != a.end(); ++it)
    {
        v.push_back(*it);
    }
    EXPECT_EQ(v, (std::vector<int>{1, 2, 3}));

    const auto &ca = a;
    EXPECT_EQ(ca.begin(), ca.cbegin());
    EXPECT_EQ(ca.end(), ca.cend());
}

TEST_F(ArrayIntTest, ReverseIterators)
{
    auto a = make123();
    std::vector<int> v;
    for (auto it = a.rbegin(); it != a.rend(); ++it)
    {
        v.push_back(*it);
    }
    EXPECT_EQ(v, (std::vector<int>{3, 2, 1}));

    const auto &ca = a;
    EXPECT_EQ(ca.rbegin(), ca.crbegin());
    EXPECT_EQ(ca.rend(), ca.crend());
}

// размерность
TEST_F(ArrayIntTest, SizeMaxSizeEmpty)
{
    Array<int, N> a;
    EXPECT_EQ(a.size(), N);
    EXPECT_EQ(a.max_size(), N);
    EXPECT_FALSE(a.empty());
}

// fill и swap

TEST_F(ArrayIntTest, Fill)
{
    auto a = make123();
    a.fill(9);
    for (auto x : a)
    {
        EXPECT_EQ(x, 9);
    }
}

TEST_F(ArrayIntTest, Swap)
{
    auto a = make123();
    auto b = make132();
    a.swap(b);
    EXPECT_EQ(a, make132());
    EXPECT_EQ(b, make123());
}

// сравнения

TEST_F(ArrayIntTest, Comparisons)
{
    auto a = make123();
    auto b = make123();
    auto c = make132();

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    EXPECT_LT(a, c);
    EXPECT_LE(a, c);
    EXPECT_GT(c, a);
    EXPECT_GE(c, a);

    EXPECT_FALSE(c < a);
    EXPECT_FALSE(a > c);
}

TEST_F(ArrayStringTest, DefaultCtor_ValueInitialized)
{
    Array<std::string, N> a;
    EXPECT_EQ(a.size(), N);
    for (auto const &s : a)
    {
        EXPECT_EQ(s, std::string{});
    }
}

TEST_F(ArrayStringTest, InitListCtor_ExactSize)
{
    auto a = makeABC();
    EXPECT_EQ(a.size(), N);
    EXPECT_EQ(a.front(), "a");
    EXPECT_EQ(a.back(), "c");
}

TEST_F(ArrayStringTest, InitListCtor_WrongSizeThrows)
{
    EXPECT_THROW((Array<std::string, N>{"a", "b"}), std::length_error);
    EXPECT_THROW((Array<std::string, N>{"a", "b", "c", "d"}), std::length_error);
}

TEST_F(ArrayStringTest, CopyMoveAssignAndAccess)
{
    auto a = makeABC();
    Array<std::string, N> b;
    b = a;
    EXPECT_EQ(a, b);

    Array<std::string, N> c{std::move(a)};
    EXPECT_EQ(c.size(), N);

    c[0] = "x";
    EXPECT_EQ(c.front(), "x");
    EXPECT_NO_THROW((void)c.at(2));
    EXPECT_THROW((void)c.at(3), std::out_of_range);
}

TEST_F(ArrayStringTest, IteratorsAndFillSwap)
{
    auto a = makeABC();

    std::vector<std::string> v;
    for (auto it = a.begin(); it != a.end(); ++it)
    {
        v.push_back(*it);
    }
    EXPECT_EQ(v, (std::vector<std::string>{"a", "b", "c"}));

    const auto &ca = a;
    EXPECT_EQ(ca.begin(), ca.cbegin());
    EXPECT_EQ(ca.end(), ca.cend());

    std::vector<std::string> rv;
    for (auto it = a.rbegin(); it != a.rend(); ++it)
    {
        rv.push_back(*it);
    }
    EXPECT_EQ(rv, (std::vector<std::string>{"c", "b", "a"}));

    a.fill("z");
    for (auto const &s : a)
    {
        EXPECT_EQ(s, "z");
    }

    auto b = makeACB();
    a.swap(b);
    EXPECT_EQ(a, makeACB());
}

TEST_F(ArrayStringTest, Comparisons)
{
    auto a = makeABC();
    auto b = makeABC();
    auto c = makeACB();

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    EXPECT_LT(a, c);
    EXPECT_LE(a, c);
    EXPECT_GT(c, a);
    EXPECT_GE(c, a);
}

// тесты для N == 0

TEST(ArrayZero, Basics)
{
    Array<int, 0> z;
    EXPECT_EQ(z.size(), 0u);
    EXPECT_EQ(z.max_size(), 0u);
    EXPECT_TRUE(z.empty());

    EXPECT_EQ(z.begin(), nullptr);
    EXPECT_EQ(z.end(), nullptr);
    EXPECT_EQ(z.cbegin(), nullptr);
    EXPECT_EQ(z.cend(), nullptr);
    EXPECT_EQ(z.rbegin(), z.rend());
    EXPECT_EQ(z.crbegin(), z.crend());

    EXPECT_NO_THROW((Array<int, 0>{}));
    EXPECT_THROW((Array<int, 0>{1}), std::length_error);
}

TEST(ArrayZero, AccessThrows)
{
    Array<int, 0> z;
    EXPECT_THROW((void)z.at(0), std::out_of_range);
}

TEST(ArrayZero, ComparisonsAndOps)
{
    Array<int, 0> a, b;
    EXPECT_EQ(a, b);
    EXPECT_LE(a, b);
    EXPECT_GE(a, b);
    a.swap(b);
    a.fill(42);
}
