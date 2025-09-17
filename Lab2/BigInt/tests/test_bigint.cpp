#include "bigint.hpp"
#include <gtest/gtest.h>
#include <sstream>

using core::BigInt;

struct BigIntFixture : ::testing::Test
{
    BigInt zero;
    BigInt one{1};
    BigInt ten{10};
    BigInt bigA{"123456789123456789123456789"};
    BigInt bigB{"987654321987654321"};
};

TEST_F(BigIntFixture, ConstructionFromLLAndString)
{
    BigInt a(0);
    BigInt b(-0);
    BigInt c("-0000");
    BigInt d("-42");
    BigInt e("42");
    EXPECT_EQ(a.to_string(), "0");
    EXPECT_EQ(b.to_string(), "0");
    EXPECT_EQ(c.to_string(), "0");
    EXPECT_EQ(d.to_string(), "-42");
    EXPECT_EQ(e.to_string(), "42");
}

TEST_F(BigIntFixture, ComparisonBasics)
{
    EXPECT_TRUE(BigInt(0) == BigInt(0));
    EXPECT_TRUE(BigInt(1) != BigInt(0));
    EXPECT_TRUE(BigInt(-1) < BigInt(0));
    EXPECT_TRUE(BigInt(0) < BigInt(1));
    EXPECT_TRUE(BigInt(-2) < BigInt(-1));
    EXPECT_TRUE(BigInt(2) > BigInt(1));
    EXPECT_TRUE(BigInt(-3) <= BigInt(-3));
    EXPECT_TRUE(BigInt(5) >= BigInt(5));
}

TEST_F(BigIntFixture, AdditionSimple)
{
    EXPECT_EQ((BigInt(2) + BigInt(3)).to_string(), "5");
    EXPECT_EQ((BigInt(-2) + BigInt(3)).to_string(), "1");
    EXPECT_EQ((BigInt(2) + BigInt(-3)).to_string(), "-1");
    EXPECT_EQ((BigInt(-2) + BigInt(-3)).to_string(), "-5");
    EXPECT_EQ((bigA + bigB).to_string(), "123456790111111111111111110");
}

TEST_F(BigIntFixture, SubtractionSimple)
{
    EXPECT_EQ((BigInt(5) - BigInt(3)).to_string(), "2");
    EXPECT_EQ((BigInt(3) - BigInt(5)).to_string(), "-2");
    EXPECT_EQ((BigInt(-3) - BigInt(5)).to_string(), "-8");
    EXPECT_EQ((BigInt(-5) - BigInt(-3)).to_string(), "-2");
    EXPECT_EQ((bigA - bigB).to_string(), "123456788135802467135802468");
}

TEST_F(BigIntFixture, MultiplicationSimple)
{
    EXPECT_EQ((BigInt(0) * BigInt(123)).to_string(), "0");
    EXPECT_EQ((BigInt(-2) * BigInt(3)).to_string(), "-6");
    EXPECT_EQ((BigInt(-2) * BigInt(-3)).to_string(), "6");
    EXPECT_EQ((BigInt("999999999") * BigInt("999999999")).to_string(), "999999998000000001");
    EXPECT_EQ((bigA * bigB).to_string(), "121932631356500531469135800347203169112635269");
}

TEST_F(BigIntFixture, DivisionSimple)
{
    EXPECT_EQ((BigInt(10) / BigInt(2)).to_string(), "5");
    EXPECT_EQ((BigInt(10) / BigInt(3)).to_string(), "3");
    EXPECT_EQ((BigInt(-10) / BigInt(3)).to_string(), "-3");
    EXPECT_EQ((BigInt(10) / BigInt(-3)).to_string(), "-3");
    EXPECT_EQ((BigInt(-10) / BigInt(-3)).to_string(), "3");
    EXPECT_EQ((BigInt("1000000000000000000") / BigInt("1000000000")).to_string(), "1000000000");
}

TEST_F(BigIntFixture, DivisionByZeroThrows)
{
    EXPECT_THROW(BigInt(1) / BigInt(0), std::domain_error);
    BigInt x(1);
    EXPECT_THROW(x /= BigInt(0), std::domain_error);
}

TEST_F(BigIntFixture, ChainedOps)
{
    BigInt x("12345678901234567890");
    BigInt y("987654321");
    BigInt z = ((x + y) * BigInt(2)) / BigInt(3);
    std::string s = z.to_string();
    EXPECT_FALSE(s.empty());
}

TEST_F(BigIntFixture, IOStreams)
{
    std::stringstream ss;
    ss << BigInt("-0012345");
    EXPECT_EQ(ss.str(), "-12345");

    std::stringstream in(" -00001234 ");
    BigInt v;
    in >> v;
    EXPECT_EQ(v.to_string(), "-1234");
}

TEST_F(BigIntFixture, CopyAndMove)
{
    BigInt a("123456789");
    BigInt b = a;
    EXPECT_EQ(b.to_string(), "123456789");
    BigInt c = std::move(a);
    EXPECT_EQ(c.to_string(), "123456789");
    BigInt d;
    d = c;
    EXPECT_EQ(d.to_string(), "123456789");
    BigInt e;
    e = std::move(d);
    EXPECT_EQ(e.to_string(), "123456789");
}

TEST_F(BigIntFixture, LargeBlocksStability)
{
    BigInt a("1");
    int i = 0;
    while (i < 50)
    {
        a = a * BigInt("1000000000");
        i += 1;
    }
    BigInt b("1");
    i = 0;
    while (i < 50)
    {
        b = b * BigInt("1000000000");
        i += 1;
    }
    EXPECT_EQ(a, b);
    EXPECT_EQ((a + BigInt(1)) > a, true);
}
