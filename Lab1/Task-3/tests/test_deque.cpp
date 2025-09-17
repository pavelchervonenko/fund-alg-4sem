#include <gtest/gtest.h>
#include <numeric>
#include "my_container/deque.hpp"

using my_container::Deque;

struct DequeFixture : ::testing::Test
{
    Deque<int> d;
};

TEST_F(DequeFixture, DefaultConstruct)
{
    EXPECT_TRUE(d.empty());
    EXPECT_EQ(d.size(), 0u);
}

TEST(DequeCtor, InitList)
{
    Deque<int> d{1, 2, 3};
    ASSERT_EQ(d.size(), 3u);
    EXPECT_EQ(d.front(), 1);
    EXPECT_EQ(d.back(), 3);
}

TEST(DequeCtor, CopyConstruct)
{
    Deque<int> a{1, 2, 3, 4};
    Deque<int> b(a);
    EXPECT_EQ(a.size(), b.size());
    EXPECT_EQ(a, b);
}

TEST(DequeCtor, MoveConstruct)
{
    Deque<int> src{1, 2, 3};
    Deque<int> dst(std::move(src));
    EXPECT_EQ(dst.size(), 3u);
    // Не обращаемся к src после move — состояние не определено формально.
}

TEST(DequeAssign, CopyAssign)
{
    Deque<int> a{1, 2, 3};
    Deque<int> b;
    b = a;
    EXPECT_EQ(a, b);
    EXPECT_EQ(b.size(), 3u);
}

TEST(DequeAssign, MoveAssign)
{
    Deque<int> src{1, 2, 3};
    Deque<int> dst;
    dst = std::move(src);
    EXPECT_EQ(dst.size(), 3u);
    // Не трогаем src после move.
}

TEST_F(DequeFixture, PushPopFrontBack)
{
    d.push_back(10);
    d.push_front(5);
    d.push_back(20);
    ASSERT_EQ(d.size(), 3u);
    EXPECT_EQ(d.front(), 5);
    EXPECT_EQ(d.back(), 20);

    d.pop_front();
    EXPECT_EQ(d.front(), 10);
    d.pop_back();
    EXPECT_EQ(d.back(), 10);
    d.pop_back();
    EXPECT_TRUE(d.empty());
}

TEST_F(DequeFixture, AtAndIndex)
{
    for (int i = 0; i < 7; ++i)
        d.push_back(i * 10);
    ASSERT_EQ(d.size(), 7u);
    EXPECT_EQ(d.at(0), 0);
    EXPECT_EQ(d.at(3), 30);
    EXPECT_EQ(d.at(6), 60);
    EXPECT_EQ(d[1], 10);
    EXPECT_EQ(d[5], 50);
    EXPECT_THROW(d.at(7), std::out_of_range);
}

TEST_F(DequeFixture, IterationForward)
{
    d = Deque<int>{1, 2, 3, 4};
    int sum = std::accumulate(d.begin(), d.end(), 0);
    EXPECT_EQ(sum, 10);
}

TEST_F(DequeFixture, IterationReverse)
{
    d = Deque<int>{1, 2, 3};
    std::vector<int> v;
    for (auto it = d.rbegin(); it != d.rend(); ++it)
        v.push_back(*it);
    ASSERT_EQ(v.size(), 3u);
    EXPECT_EQ((std::vector<int>{3, 2, 1}), v);
}

TEST_F(DequeFixture, InsertErase)
{
    d = Deque<int>{1, 3, 4};
    // вставим 2 перед "3"
    auto it = d.begin();
    ++it; // указывает на 3
    d.insert(it, 2);
    EXPECT_EQ((Deque<int>{1, 2, 3, 4}), d);

    // удалим "2"
    it = d.begin();
    ++it; // снова на 2
    d.erase(Deque<int>::const_iterator(it));
    EXPECT_EQ((Deque<int>{1, 3, 4}), d);
}

TEST_F(DequeFixture, ResizeAndClear)
{
    d = Deque<int>{1, 2, 3};
    d.resize(5);
    EXPECT_EQ(d.size(), 5u);
    EXPECT_EQ(d[3], 0);
    EXPECT_EQ(d[4], 0);
    d.resize(2);
    EXPECT_EQ(d.size(), 2u);
    d.clear();
    EXPECT_TRUE(d.empty());
}

TEST(DequeCompare, EqNeLexicographic)
{
    Deque<int> a{1, 2, 3};
    Deque<int> b{1, 2, 3};
    Deque<int> c{1, 2, 4};
    Deque<int> d{1, 2, 3, 0};

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    EXPECT_LT(a, c);
    EXPECT_LE(a, b);
    EXPECT_GT(d, a);
    EXPECT_GE(d, b);
}

TEST(DequeSwap, Basic)
{
    Deque<int> a{1, 2, 3};
    Deque<int> b{9};
    a.swap(b);
    EXPECT_EQ(a.size(), 1u);
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(a.front(), 9);
    EXPECT_EQ(b.back(), 3);
}
