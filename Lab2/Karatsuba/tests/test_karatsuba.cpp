#include "bigint.hpp"
#include <gtest/gtest.h>
#include <random>
#include <sstream>
#include <string>
#include <limits>

using core::BigInt;

struct Fx : ::testing::Test
{
    std::mt19937_64 rng;
    Fx() : rng(123456789ULL) {}
    std::string num(size_t digits)
    {
        if (digits == 0)
            return std::string("0");
        std::uniform_int_distribution<int> d01(1, 9);
        std::uniform_int_distribution<int> d09(0, 9);
        std::string s;
        s.resize(digits);
        s[0] = static_cast<char>('0' + d01(rng));
        for (size_t i = 1; i < digits; i++)
            s[i] = static_cast<char>('0' + d09(rng));
        return s;
    }
};

TEST_F(Fx, ParseBasicAndZeros)
{
    BigInt a("0");
    EXPECT_EQ(a.to_string(), "0");
    BigInt b("+000123");
    EXPECT_EQ(b.to_string(), "123");
    BigInt c("   -0000  ");
    EXPECT_EQ(c.to_string(), "0");
    BigInt d("   +0012345678901234567890");
    EXPECT_EQ(d.to_string(), "12345678901234567890");
}

TEST_F(Fx, StreamInOut)
{
    std::stringstream ss;
    ss << BigInt("-12345678901234567890");
    EXPECT_EQ(ss.str(), "-12345678901234567890");
    BigInt x(0);
    std::stringstream in("   -0012345  ");
    in >> x;
    EXPECT_EQ(x.to_string(), "-12345");
}

TEST_F(Fx, CopyMoveAssign)
{
    BigInt a("123456789");
    BigInt b = a;
    EXPECT_EQ(b.to_string(), "123456789");
    BigInt c(std::move(b));
    EXPECT_EQ(c.to_string(), "123456789");
    b = BigInt("42");
    EXPECT_EQ(b.to_string(), "42");
    c = std::move(b);
    EXPECT_EQ(c.to_string(), "42");
}

TEST_F(Fx, CompareOperators)
{
    BigInt a("1000");
    BigInt b("999");
    EXPECT_TRUE(a > b);
    EXPECT_TRUE(b < a);
    EXPECT_TRUE(a >= b);
    EXPECT_TRUE(b <= a);
    EXPECT_TRUE(a != b);
    EXPECT_TRUE(a == a);
    BigInt m("-5"), n("3");
    EXPECT_TRUE(m < n);
    EXPECT_TRUE(!(n < m));
}

TEST_F(Fx, AddSubSignsAndTrim)
{
    BigInt a("1000000000");
    BigInt b("1");
    BigInt s = a + b;
    EXPECT_EQ(s.to_string(), "1000000001");
    BigInt z1("0"), z2("0");
    BigInt z3 = z1 + z2;
    EXPECT_EQ(z3.to_string(), "0");
    BigInt p("5"), q("-3");
    BigInt r = p + q;
    EXPECT_EQ(r.to_string(), "2");
    BigInt r2 = q - p;
    EXPECT_EQ(r2.to_string(), "-8");
}

TEST_F(Fx, MulSmallVsLL)
{
    for (int i = 0; i < 50; i++)
    {
        long long x = static_cast<long long>(rng() % 2000000ULL) - 1000000LL;
        long long y = static_cast<long long>(rng() % 2000000ULL) - 1000000LL;
        BigInt a(x), b(y);
        BigInt c = a * b;
        BigInt w(x * y);
        EXPECT_EQ(c.to_string(), w.to_string());
    }
}

TEST_F(Fx, MulSwitchSmallLarge)
{
    BigInt a(num(60));
    BigInt b(num(55));
    BigInt small = a * b;
    EXPECT_TRUE(small.to_string().size() >= 100u);
    BigInt c(num(320));
    BigInt d(num(310));
    BigInt large = c * d;
    EXPECT_TRUE(large.to_string().size() >= 600u);
}

TEST_F(Fx, MulSigns)
{
    BigInt a("123456789012345678901234567890");
    BigInt b("-98765432109876543210987654321");
    BigInt c = a * b;
    EXPECT_EQ(c.to_string()[0], '-');
    BigInt d = b * b;
    EXPECT_NE(d.to_string()[0], '-');
}

TEST_F(Fx, DivBasicBranches)
{
    BigInt one("1");
    BigInt three("3");
    BigInt z("0");
    EXPECT_THROW({ BigInt t = one / z; (void)t; }, std::domain_error);
    BigInt a("5"), b("10");
    BigInt q1 = a / b;
    EXPECT_EQ(q1.to_string(), "0");
    BigInt q2 = b / b;
    EXPECT_EQ(q2.to_string(), "1");
    BigInt p("12345678901234567890");
    BigInt q("12345");
    BigInt t = p / q;
    BigInt back = t * q;
    EXPECT_LE(core::BigInt::from_string(p.to_string()).to_string().size(), p.to_string().size());
    EXPECT_TRUE(core::BigInt::from_string(p.to_string()) >= back);
}

TEST_F(Fx, DivSigns)
{
    BigInt a("-1000");
    BigInt b("10");
    BigInt c = a / b;
    EXPECT_EQ(c.to_string(), "-100");
    BigInt d("1000");
    BigInt e("-10");
    BigInt f = d / e;
    EXPECT_EQ(f.to_string(), "-100");
    BigInt g("-1000"), h("-10");
    BigInt i = g / h;
    EXPECT_EQ(i.to_string(), "100");
}

TEST_F(Fx, LLongMinConstructor)
{
    long long mn = std::numeric_limits<long long>::min();
    BigInt a(mn);
    std::string s = std::to_string(mn);
    BigInt b(s);
    EXPECT_EQ(a.to_string(), b.to_string());
}

TEST_F(Fx, RandomKaratsubaProperty)
{
    for (int t = 0; t < 10; t++)
    {
        std::string s1 = num(350 + (t % 50));
        std::string s2 = num(340 + ((t * 7) % 50));
        BigInt a(s1), b(s2);
        BigInt p = a * b;
        BigInt r = p / a;
        EXPECT_EQ(r.to_string(), b.to_string());
    }
}
