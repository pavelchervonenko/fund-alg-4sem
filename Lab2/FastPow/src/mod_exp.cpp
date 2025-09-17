#include "mod_exp.hpp"
#include <stdexcept>

namespace core
{
    static BigInt mod_reduce(const BigInt &a, const BigInt &m)
    {
        BigInt q = a / m;
        BigInt r = a - q * m;
        return r;
    }

    static BigInt abs_big(const BigInt &x)
    {
        if (x < BigInt(0))
        {
            return BigInt(0) - x;
        }
        return x;
    }

    static bool is_zero(const BigInt &x)
    {
        return x == BigInt(0);
    }

    static bool is_odd(const BigInt &x)
    {
        BigInt r = mod_reduce(x, BigInt(2));
        return r == BigInt(1);
    }

    BigInt mod_exp(const BigInt &base, const BigInt &exp, const BigInt &mod)
    {
        if (mod == BigInt(0))
        {
            throw std::invalid_argument("mod is zero");
        }

        if (exp < BigInt(0))
        {
            throw std::invalid_argument("negative exponent");
        }

        BigInt m = abs_big(mod);
        BigInt a = mod_reduce(base, m);
        if (a < BigInt(0))
        {
            a = a + m;
        }

        BigInt e = exp;
        BigInt result(1);

        while (!is_zero(e))
        {
            if (is_odd(e))
            {
                result = mod_reduce(result * a, m);
            }

            a = mod_reduce(a * a, m);
            e = e / BigInt(2);
        }

        return result;
    }
}
