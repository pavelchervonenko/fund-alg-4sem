#include "my_container/list.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <algorithm> // std::copy
#include <iterator>  // std::back_inserter

using my_container::List;

// -------------------- фикстуры --------------------
class ListIntTest : public ::testing::Test
{
protected:
    using T = int;

    List<T> make123() const
    {
        // explicit ctor требует явного указания типа
        return List<T>{1, 2, 3};
    }
};

class ListStringTest : public ::testing::Test
{
protected:
    using T = std::string;

    List<T> makeABC() const
    {
        // explicit ctor требует явного указания типа
        return List<T>{"a", "b", "c"};
    }
};

// -------------------- конструкторы --------------------
TEST_F(ListIntTest, DefaultCtorEmpty)
{
    List<int> l;
    EXPECT_TRUE(l.empty());
    EXPECT_EQ(l.size(), 0u);
}

TEST_F(ListIntTest, InitListCtor)
{
    auto l = make123();
    EXPECT_EQ(l.size(), 3u);
    EXPECT_EQ(l.front(), 1);
    EXPECT_EQ(l.back(), 3);
}

TEST_F(ListIntTest, CopyCtorAndAssign)
{
    auto a = make123();
    List<int> b(a);
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b.front(), 1);

    List<int> c;
    c = a;
    EXPECT_EQ(c.size(), 3u);
    EXPECT_EQ(c.back(), 3);
}

TEST_F(ListIntTest, MoveCtorAndAssign)
{
    auto a = make123();
    List<int> b(std::move(a));
    EXPECT_EQ(b.size(), 3u);

    List<int> c;
    c = std::move(b);
    EXPECT_EQ(c.size(), 3u);
    // cppcheck-suppress accessMoved
    EXPECT_TRUE(b.empty());
}

// -------------------- методы доступа --------------------
TEST_F(ListIntTest, PushPopFrontBack)
{
    List<int> l;
    l.push_back(1);
    l.push_front(0);
    EXPECT_EQ(l.front(), 0);
    EXPECT_EQ(l.back(), 1);

    l.pop_back();
    EXPECT_EQ(l.back(), 0);
    l.pop_front();
    EXPECT_TRUE(l.empty());
}

// -------------------- итераторы --------------------
TEST_F(ListIntTest, ForwardIteration)
{
    auto l = make123();
    std::vector<int> v;
    std::copy(l.begin(), l.end(), std::back_inserter(v));
    EXPECT_EQ(v, (std::vector<int>{1, 2, 3}));
}

TEST_F(ListIntTest, ConstForwardIteration)
{
    const auto l = make123();
    std::vector<int> v;
    std::copy(l.cbegin(), l.cend(), std::back_inserter(v));
    EXPECT_EQ(v.size(), 3u);
}

TEST_F(ListIntTest, ReverseIteration)
{
    auto l = make123();
    std::vector<int> v;
    std::copy(l.rbegin(), l.rend(), std::back_inserter(v));
    EXPECT_EQ(v, (std::vector<int>{3, 2, 1}));
}

TEST_F(ListIntTest, ConstReverseIteration)
{
    const auto l = make123();
    std::vector<int> v;
    std::copy(l.crbegin(), l.crend(), std::back_inserter(v));
    EXPECT_EQ(v.front(), 3);
}

// -------------------- модификаторы --------------------
TEST_F(ListIntTest, ClearResize)
{
    auto l = make123();
    l.clear();
    EXPECT_TRUE(l.empty());

    // В контейнере реализована resize(count) — добавляем default-значения
    l.resize(5);
    EXPECT_EQ(l.size(), 5u);

    // проверим, что появились нули (default int{})
    std::vector<int> v;
    std::copy(l.begin(), l.end(), std::back_inserter(v));
    EXPECT_EQ(v, (std::vector<int>{0, 0, 0, 0, 0}));
}

TEST_F(ListIntTest, InsertErase)
{
    auto l = make123(); // 1 2 3

    auto it = l.begin();
    ++it; // на элемент 2

    // после explicit-конструктора нужно явно строить const_iterator
    l.insert(typename List<int>::const_iterator(it), 9); // 1 9 2 3
    EXPECT_EQ(l.size(), 4u);

    it = l.begin();
    ++it;                                            // на 9
    l.erase(typename List<int>::const_iterator(it)); // 1 2 3
    EXPECT_EQ(l.size(), 3u);
}

TEST_F(ListIntTest, Swap)
{
    auto a = make123();
    List<int> b = List<int>{7, 8};
    a.swap(b);

    std::vector<int> va, vb;
    std::copy(a.begin(), a.end(), std::back_inserter(va));
    std::copy(b.begin(), b.end(), std::back_inserter(vb));

    EXPECT_EQ(va, (std::vector<int>{7, 8}));
    EXPECT_EQ(vb, (std::vector<int>{1, 2, 3}));
}

// -------------------- сравнения --------------------
TEST_F(ListIntTest, Comparisons)
{
    auto a = make123();
    auto b = make123();
    auto c = List<int>{1, 2, 4};

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);
    EXPECT_TRUE(a < c);
    EXPECT_TRUE(c > a);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(a >= b);
}

// -------------------- строки --------------------
TEST_F(ListStringTest, InitListCtor)
{
    auto l = makeABC();
    EXPECT_EQ(l.size(), 3u);
    EXPECT_EQ(l.front(), "a");
    EXPECT_EQ(l.back(), "c");
}

TEST_F(ListStringTest, ReverseIteration)
{
    auto l = makeABC();
    std::vector<std::string> v;
    std::copy(l.rbegin(), l.rend(), std::back_inserter(v));
    EXPECT_EQ(v, (std::vector<std::string>{"c", "b", "a"}));
}

TEST_F(ListStringTest, Comparisons)
{
    auto a = makeABC();
    auto b = makeABC();
    auto c = List<std::string>{"a", "b", "d"};

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a < c);
}

// -------------------- исключения на пустом --------------------
TEST(ListEmpty, OperationsThrowOnEmpty)
{
    List<int> l;
    EXPECT_THROW((void)l.front(), std::out_of_range);
    EXPECT_THROW((void)l.back(), std::out_of_range);
    EXPECT_THROW(l.pop_front(), std::out_of_range);
    EXPECT_THROW(l.pop_back(), std::out_of_range);
}

TEST(ListEmpty, EraseEndThrows)
{
    List<int> l;
    EXPECT_THROW((void)l.erase(l.cend()), std::out_of_range);
}
