#include <gtest/gtest.h>
#include "my_container/my_unique_ptr.hpp"

using my_smart_ptr::UniquePtr;

struct DtorProbe
{
    int *dtor_counter;
    int x{0};
    explicit DtorProbe(int *ctr) : dtor_counter(ctr) {}
    ~DtorProbe()
    {
        if (dtor_counter)
            ++(*dtor_counter);
    }
    void bump() { ++x; }
};

class UniquePtrTest : public ::testing::Test
{
};

TEST_F(UniquePtrTest, DefaultCtor)
{
    UniquePtr<int> p;
    EXPECT_FALSE(p);
    EXPECT_EQ(p.get(), nullptr);
}

TEST_F(UniquePtrTest, FromPointer)
{
    auto *raw = new int(42);
    UniquePtr<int> p(raw);
    EXPECT_TRUE(p);
    EXPECT_EQ(p.get(), raw);
    EXPECT_EQ(*p, 42);
}

TEST_F(UniquePtrTest, MoveCtor)
{
    auto *raw = new int(7);
    {
        UniquePtr<int> a(raw);
        UniquePtr<int> b(std::move(a));
        EXPECT_TRUE(b);
        EXPECT_EQ(b.get(), raw);
        EXPECT_EQ(*b, 7);
    }
}

TEST_F(UniquePtrTest, MoveAssignReleasesOld)
{
    int dcount = 0;
    auto *raw1 = new DtorProbe(&dcount);
    auto *raw2 = new DtorProbe(&dcount);
    {
        UniquePtr<DtorProbe> a(raw1);
        UniquePtr<DtorProbe> b(raw2);
        b = std::move(a);
        EXPECT_EQ(dcount, 1);
        EXPECT_EQ(b.get(), raw1);
        EXPECT_TRUE(b);
    }
    EXPECT_EQ(dcount, 2);
}

TEST_F(UniquePtrTest, Accessors)
{
    auto *raw = new int(10);
    UniquePtr<int> p(raw);
    EXPECT_EQ(p.get(), raw);
    EXPECT_TRUE(static_cast<bool>(p));
    EXPECT_EQ(*p, 10);
}

TEST_F(UniquePtrTest, ArrowOperator)
{
    int dcount = 0;
    auto *raw = new DtorProbe(&dcount);
    UniquePtr<DtorProbe> p(raw);
    p->bump();
    EXPECT_EQ((*p).x, 1);
}

TEST_F(UniquePtrTest, Release)
{
    auto *raw = new int(5);
    UniquePtr<int> p(raw);
    int *out = p.release();
    EXPECT_EQ(out, raw);
    EXPECT_FALSE(p);
    delete out;
}

TEST_F(UniquePtrTest, ResetDeletesOld)
{
    int dcount = 0;
    auto *a = new DtorProbe(&dcount);
    auto *b = new DtorProbe(&dcount);
    {
        UniquePtr<DtorProbe> p(a);
        p.reset(b);
        EXPECT_EQ(p.get(), b);
    }
    EXPECT_EQ(dcount, 2);
}

TEST_F(UniquePtrTest, ResetNull)
{
    int dcount = 0;
    auto *a = new DtorProbe(&dcount);
    {
        UniquePtr<DtorProbe> p(a);
        p.reset();
        EXPECT_FALSE(p);
    }
    EXPECT_EQ(dcount, 1);
}

TEST_F(UniquePtrTest, Swap)
{
    auto *a = new int(1);
    auto *b = new int(2);
    UniquePtr<int> p(a);
    UniquePtr<int> q(b);
    p.swap(q);
    EXPECT_EQ(*p, 2);
    EXPECT_EQ(*q, 1);
}

TEST_F(UniquePtrTest, ArrayBasic)
{
    UniquePtr<int[]> p(new int[3]{1, 2, 3});
    ASSERT_TRUE(p);
    EXPECT_EQ(p[0], 1);
    EXPECT_EQ(p[1], 2);
    EXPECT_EQ(p[2], 3);
}

TEST_F(UniquePtrTest, ArrayRelease)
{
    UniquePtr<int[]> p(new int[2]{10, 20});
    int *raw = p.release();
    EXPECT_FALSE(p);
    EXPECT_EQ(raw[0], 10);
    EXPECT_EQ(raw[1], 20);
    delete[] raw;
}

TEST_F(UniquePtrTest, ArrayReset)
{
    UniquePtr<int[]> p(new int[2]{1, 2});
    p.reset(new int[3]{7, 8, 9});
    EXPECT_EQ(p[0], 7);
    EXPECT_EQ(p[1], 8);
    EXPECT_EQ(p[2], 9);
}

struct ArrProbe
{
    static int alive;
    ArrProbe() { ++alive; }
    ~ArrProbe() { --alive; }
};
int ArrProbe::alive = 0;

TEST_F(UniquePtrTest, ArrayDestructor)
{
    {
        UniquePtr<ArrProbe[]> p(new ArrProbe[5]);
        EXPECT_EQ(ArrProbe::alive, 5);
    }
    EXPECT_EQ(ArrProbe::alive, 0);
}
