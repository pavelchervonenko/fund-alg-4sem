#pragma once
#include "bigint.hpp"

namespace core
{
    BigInt mod_exp(const BigInt &base, const BigInt &exp, const BigInt &mod);
}
