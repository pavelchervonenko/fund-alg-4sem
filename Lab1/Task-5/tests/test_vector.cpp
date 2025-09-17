#include <gtest/gtest.h>

#include <compare>
#include <stdexcept>
#include <vector>

#include "my_container/vector.hpp"

using my_container::Vector;

// Вспомогательный тип для проверки, что при реаллокациях преобладают перемещения
struct MoveCounter
{
    int v{0};

    inline static int moves = 0;
    inline static int copies = 0;

    MoveCounter() = default;
    explicit MoveCounter(int x) : v(x) {}

    MoveCounter(const MoveCounter &other) : v(other.v)
    {
        ++copies;
    }

    MoveCounter(MoveCounter &&other) noexcept : v(other.v)
    {
        ++moves;
    }

    MoveCounter &operator=(const MoveCounter &other)
    {
        v = other.v;
        ++copies;
        return *this;
    }

    MoveCounter &operator=(MoveCounter &&other) noexcept
    {
        v = other.v;
        ++moves;
        return *this;
    }

    auto operator<=>(const MoveCounter &) const = default;
    bool operator==(const MoveCounter &) const = default;
};

// Фикстура, чтобы не дублировать инициализацию
struct VectorIntFixture : ::testing::Test
{
    Vector<int> a;
    Vector<int> b;

    // cppcheck-suppress unusedFunction
    void SetUp() override
    {
        a = {1, 2, 3};
        b = {1, 2, 3};
    }
};

// ---------- Конструкторы ----------

TEST(Vector_Construct, Default)
{
    Vector<int> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0u);
    EXPECT_EQ(v.capacity(), 0u);
    EXPECT_EQ(v.data(), nullptr);
}

TEST(Vector_Construct, InitList)
{
    Vector<int> v{1, 2, 3, 4};
    EXPECT_EQ(v.size(), 4u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[3], 4);
}

TEST(Vector_Construct, Copy)
{
    Vector<int> v{1, 2, 3};
    Vector<int> u = v;
    EXPECT_EQ(u.size(), 3u);
    EXPECT_EQ(u, v);
}

TEST(Vector_Construct, Move)
{
    Vector<int> v{1, 2, 3};
    Vector<int> u = std::move(v);
    EXPECT_EQ(u.size(), 3u);
}

// ---------- Присваивание ----------

TEST(Vector_Assign, CopyAssign)
{
    Vector<int> v{1, 2, 3};
    Vector<int> u;
    u = v;
    EXPECT_EQ(u, v);
}

TEST(Vector_Assign, MoveAssign)
{
    Vector<int> v{1, 2, 3};
    Vector<int> u;
    u = std::move(v);
    EXPECT_EQ(u.size(), 3u);
}

TEST(Vector_Assign, InitListAssign)
{
    Vector<int> v;
    v = {10, 20, 30};
    ASSERT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
    EXPECT_EQ(v[2], 30);
}

// ---------- Доступ ----------

TEST(Vector_Access, AtBounds)
{
    Vector<int> v{10, 20, 30};
    EXPECT_EQ(v.at(1), 20);
    EXPECT_THROW((void)v.at(3), std::out_of_range);
}

TEST(Vector_Access, FrontBackData)
{
    Vector<int> v{7, 8, 9};
    EXPECT_EQ(v.front(), 7);
    EXPECT_EQ(v.back(), 9);
    ASSERT_NE(v.data(), nullptr);
}

// ---------- Сравнения ----------

TEST_F(VectorIntFixture, EqualityAndOrdering)
{
    EXPECT_EQ(a, b);

    b[2] = 99;
    EXPECT_NE(a, b);
    EXPECT_LT(a, b);
    EXPECT_LE(a, b);
    EXPECT_GT(b, a);
    EXPECT_GE(b, a);
}

TEST(Vector_Spaceship, ThreeWay)
{
    Vector<int> x{1, 2, 3};
    Vector<int> y{1, 2, 4};

    auto c1 = (x <=> y);
    auto c2 = (y <=> x);
    auto c3 = (x <=> x);

    EXPECT_TRUE(std::is_lt(c1)); // x < y
    EXPECT_TRUE(std::is_gt(c2)); // y > x
    EXPECT_TRUE(std::is_eq(c3)); // x == x
}

// ---------- Емкость ----------

TEST(Vector_Capacity, ReserveAndShrink)
{
    Vector<int> v;
    v.reserve(10);
    EXPECT_GE(v.capacity(), 10u);

    v.push_back(1);
    v.push_back(2);
    v.shrink_to_fit();

    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v.capacity(), v.size());
}

// ---------- Модификаторы ----------

TEST(Vector_Modify, PushPopClear)
{
    Vector<int> v;
    v.push_back(5);
    v.push_back(6);

    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v.back(), 6);

    v.pop_back();
    EXPECT_EQ(v.size(), 1u);
    EXPECT_EQ(v.back(), 5);

    v.clear();
    EXPECT_TRUE(v.empty());
    EXPECT_THROW(v.pop_back(), std::out_of_range);
}

TEST(Vector_Modify, InsertEraseMiddle)
{
    Vector<int> v{1, 3, 4};

    auto it = v.insert(v.begin() + 1, 2);
    EXPECT_EQ(*it, 2);

    std::vector<int> snapshot(v.begin(), v.end());
    EXPECT_EQ(snapshot, (std::vector<int>{1, 2, 3, 4}));

    auto it2 = v.erase(v.begin() + 2); // удалить 3
    EXPECT_EQ(*it2, 4);

    snapshot.assign(v.begin(), v.end());
    EXPECT_EQ(snapshot, (std::vector<int>{1, 2, 4}));
}

TEST(Vector_Modify, Resize)
{
    Vector<int> v{1, 2, 3};

    v.resize(5);
    EXPECT_EQ(v.size(), 5u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    EXPECT_EQ(v[3], 0);
    EXPECT_EQ(v[4], 0);

    v.resize(2);
    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
}

TEST(Vector_Swap, SwapBasic)
{
    Vector<int> a{1, 2, 3};
    Vector<int> b{9, 8};

    a.swap(b);

    std::vector<int> sa(a.begin(), a.end());
    std::vector<int> sb(b.begin(), b.end());

    EXPECT_EQ(sa, (std::vector<int>{9, 8}));
    EXPECT_EQ(sb, (std::vector<int>{1, 2, 3}));
}

// ---------- Исключения ----------

TEST(Vector_Exception, EraseEndInvalid)
{
    Vector<int> v{1, 2, 3};
    EXPECT_THROW((void)v.erase(v.end()), std::out_of_range);
}

// ---------- Поведение перемещений/копирований ----------

TEST(Vector_Perf, MoveVsCopyOnRealloc)
{
    MoveCounter::moves = 0;
    MoveCounter::copies = 0;

    Vector<MoveCounter> v;
    for (int i = 0; i < 64; ++i)
    {
        v.push_back(MoveCounter{i});
    }

    // Для типов с nothrow move ожидаем, что перемещений не меньше копий
    EXPECT_GE(MoveCounter::moves, MoveCounter::copies);
}
