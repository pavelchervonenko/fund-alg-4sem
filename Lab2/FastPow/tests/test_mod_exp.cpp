#include <gtest/gtest.h>
#include "bigint.hpp"
#include "mod_exp.hpp"

using core::BigInt;
using core::mod_exp;

static BigInt mod_reduce(const BigInt &a, const BigInt &m)
{
    BigInt q = a / m;
    BigInt r = a - q * m;
    return r;
}

static BigInt slow_pow_mod(const BigInt &base_in, BigInt exp, const BigInt &mod)
{
    if (mod == BigInt(0))
    {
        throw std::invalid_argument("mod is zero");
    }

    BigInt m = mod;
    if (m < BigInt(0))
    {
        m = BigInt(0) - m;
    }

    BigInt b = mod_reduce(base_in, m);
    if (b < BigInt(0))
    {
        b = b + m;
    }

    BigInt res(1);
    BigInt one(1);
    while (exp > BigInt(0))
    {
        res = mod_reduce(res * b, m);
        exp = exp - one;
    }
    return res;
}

TEST(ModExpBasic, SmallIntegers)
{
    EXPECT_EQ(mod_exp(BigInt(2), BigInt(3), BigInt(5)), BigInt(3));
    EXPECT_EQ(mod_exp(BigInt(3), BigInt(3), BigInt(7)), BigInt(6));
    EXPECT_EQ(mod_exp(BigInt(10), BigInt(0), BigInt(7)), BigInt(1));
}

TEST(ModExpBasic, ModOne)
{
    EXPECT_EQ(mod_exp(BigInt(123), BigInt(456), BigInt(1)), BigInt(0));
}

TEST(ModExpBasic, NegativeBase)
{
    EXPECT_EQ(mod_exp(BigInt(-2), BigInt(3), BigInt(5)), BigInt(2));
    EXPECT_EQ(mod_exp(BigInt(-2), BigInt(2), BigInt(5)), BigInt(4));
}

TEST(ModExpBasic, LargePowers)
{
    BigInt b("123456789012345678901234567890");
    BigInt e("12345");
    BigInt m("1000000007");
    BigInt r = mod_exp(b, e, m);
    EXPECT_EQ(r, BigInt("883043778"));
}

TEST(ModExpEdges, ZeroExponent)
{
    EXPECT_EQ(mod_exp(BigInt(5), BigInt(0), BigInt(13)), BigInt(1));
}

TEST(ModExpEdges, ZeroMod)
{
    EXPECT_THROW(mod_exp(BigInt(5), BigInt(3), BigInt(0)), std::invalid_argument);
}

TEST(ModExpEdges, NegativeExponent)
{
    EXPECT_THROW(mod_exp(BigInt(2), BigInt(-3), BigInt(5)), std::invalid_argument);
}

TEST(ModExpEdges, NegativeModNormalizes)
{
    EXPECT_EQ(mod_exp(BigInt(2), BigInt(5), BigInt(-7)), BigInt(4));
}

TEST(ModExpEdges, BaseReduction)
{
    EXPECT_EQ(mod_exp(BigInt(1000), BigInt(3), BigInt(10)), BigInt(0));
    EXPECT_EQ(mod_exp(BigInt(1001), BigInt(3), BigInt(10)), BigInt(1));
}

TEST(ModExpRandom, CrossCheckSmall)
{
    for (int a = -3; a <= 7; ++a)
    {
        for (int e = 0; e <= 7; ++e)
        {
            for (int m : {2, 3, 5, 7, 11, 13, 97})
            {
                BigInt B(a);
                BigInt E(e);
                BigInt M(m);
                EXPECT_EQ(mod_exp(B, E, M), slow_pow_mod(B, E, M)) << a << " " << e << " " << m;
            }
        }
    }
}

TEST(ModExpRandom, CrossCheckMedium)
{
    BigInt base("12345678901234567890");
    BigInt exp("123");
    BigInt mod("10007");
    EXPECT_EQ(mod_exp(base, exp, mod), slow_pow_mod(base, exp, mod));
}
