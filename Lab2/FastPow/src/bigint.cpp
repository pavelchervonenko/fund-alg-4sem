#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <limits>

#include "bigint.hpp"

namespace core
{

    BigInt::BigInt() : d(), neg(false)
    {
    }

    BigInt::BigInt(long long v) : d(), neg(false)
    {
        if (v < 0)
        {
            neg = true;
            if (v == std::numeric_limits<long long>::min())
            {
                long long q = v / 10;
                long long r = v % 10;
                if (q < 0)
                {
                    q = -q;
                }
                if (r < 0)
                {
                    r = -r;
                }
                BigInt t(q);
                t = t * BigInt(10) + BigInt(r);
                *this = t;
                neg = true;
                return;
            }
            v = -v;
        }
        while (v > 0)
        {
            uint32_t block = static_cast<uint32_t>(v % BASE);
            d.push_back(block);
            v /= BASE;
        }
        trim();
    }

    BigInt::BigInt(const std::string &s)
    {
        *this = from_string(s);
    }

    BigInt BigInt::from_string(const std::string &s)
    {
        BigInt r;
        r.d.clear();
        std::string t;
        size_t i = 0;
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])) != 0)
        {
            i += 1;
        }
        bool is_neg = false;
        if (i < s.size())
        {
            if (s[i] == '+')
            {
                i += 1;
            }
            else if (s[i] == '-')
            {
                is_neg = true;
                i += 1;
            }
        }
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])) != 0)
        {
            i += 1;
        }
        while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])) != 0)
        {
            t.push_back(s[i]);
            i += 1;
        }
        size_t j = 0;
        while (j < t.size() && t[j] == '0')
        {
            j += 1;
        }
        if (j == t.size())
        {
            r.neg = false;
            r.d.clear();
            return r;
        }
        for (size_t p = t.size(); p > j;)
        {
            size_t start = p >= BASE_DIGS ? p - BASE_DIGS : 0;
            if (start < j)
            {
                start = j;
            }
            size_t len = p - start;
            uint32_t block = 0;
            for (size_t k = start; k < start + len; ++k)
            {
                block = block * 10u + static_cast<uint32_t>(t[k] - '0');
            }
            r.d.push_back(block);
            p = start;
        }
        r.neg = is_neg;
        r.trim();
        return r;
    }

    BigInt::BigInt(const BigInt &other) : d(other.d), neg(other.neg)
    {
    }

    BigInt::BigInt(BigInt &&other) noexcept : d(std::move(other.d)), neg(other.neg)
    {
        other.neg = false;
    }

    BigInt &BigInt::operator=(const BigInt &other)
    {
        if (this != &other)
        {
            d = other.d;
            neg = other.neg;
        }
        return *this;
    }

    BigInt &BigInt::operator=(BigInt &&other) noexcept
    {
        if (this != &other)
        {
            d = std::move(other.d);
            neg = other.neg;
            other.neg = false;
        }
        return *this;
    }

    void BigInt::trim()
    {
        while (!d.empty() && d.back() == 0u)
        {
            d.pop_back();
        }
        if (d.empty())
        {
            neg = false;
        }
    }

    int BigInt::cmp_abs(const BigInt &a, const BigInt &b)
    {
        if (a.d.size() > b.d.size())
        {
            return 1;
        }
        if (a.d.size() < b.d.size())
        {
            return -1;
        }
        size_t n = a.d.size();
        while (n > 0)
        {
            uint32_t x = a.d[n - 1];
            uint32_t y = b.d[n - 1];
            if (x > y)
            {
                return 1;
            }
            if (x < y)
            {
                return -1;
            }
            n -= 1;
        }
        return 0;
    }

    BigInt BigInt::add_abs(const BigInt &a, const BigInt &b)
    {
        BigInt r;
        size_t n = a.d.size();
        size_t m = b.d.size();
        size_t sz = n > m ? n : m;
        r.d.resize(sz);
        uint64_t carry = 0;
        size_t i = 0;
        while (i < sz)
        {
            uint64_t x = 0;
            uint64_t y = 0;
            if (i < n)
            {
                x = a.d[i];
            }
            if (i < m)
            {
                y = b.d[i];
            }
            uint64_t s = x + y + carry;
            r.d[i] = static_cast<uint32_t>(s % BASE);
            carry = s / BASE;
            i += 1;
        }
        if (carry > 0)
        {
            r.d.push_back(static_cast<uint32_t>(carry));
        }
        r.neg = false;
        r.trim();
        return r;
    }

    BigInt BigInt::sub_abs(const BigInt &a, const BigInt &b)
    {
        BigInt r;
        r.d.resize(a.d.size());
        int64_t carry = 0;
        size_t i = 0;
        while (i < a.d.size())
        {
            int64_t x = a.d[i];
            int64_t y = 0;
            if (i < b.d.size())
            {
                y = b.d[i];
            }
            int64_t s = x - y - carry;
            if (s < 0)
            {
                s += BASE;
                carry = 1;
            }
            else
            {
                carry = 0;
            }
            r.d[i] = static_cast<uint32_t>(s);
            i += 1;
        }
        r.neg = false;
        r.trim();
        return r;
    }

    BigInt BigInt::mul_abs(const BigInt &a, const BigInt &b)
    {
        BigInt r;
        if (a.d.empty() || b.d.empty())
        {
            return r;
        }
        r.d.assign(a.d.size() + b.d.size(), 0u);
        size_t i = 0;
        while (i < a.d.size())
        {
            uint64_t carry = 0;
            uint64_t xi = a.d[i];
            size_t j = 0;
            while (j < b.d.size())
            {
                uint64_t cur = r.d[i + j] + xi * static_cast<uint64_t>(b.d[j]) + carry;
                r.d[i + j] = static_cast<uint32_t>(cur % BASE);
                carry = cur / BASE;
                j += 1;
            }
            size_t pos = i + b.d.size();
            while (carry > 0)
            {
                uint64_t cur = r.d[pos] + carry;
                r.d[pos] = static_cast<uint32_t>(cur % BASE);
                carry = cur / BASE;
                pos += 1;
            }
            i += 1;
        }
        r.neg = false;
        r.trim();
        return r;
    }

    bool BigInt::sub_inplace(std::vector<uint32_t> &x, const std::vector<uint32_t> &y)
    {
        if (x.size() < y.size())
        {
            return false;
        }
        int64_t carry = 0;
        size_t i = 0;
        while (i < x.size())
        {
            int64_t a = x[i];
            int64_t b = 0;
            if (i < y.size())
            {
                b = y[i];
            }
            int64_t s = a - b - carry;
            if (s < 0)
            {
                s += BASE;
                carry = 1;
            }
            else
            {
                carry = 0;
            }
            x[i] = static_cast<uint32_t>(s);
            i += 1;
        }
        if (carry != 0)
        {
            return false;
        }
        while (!x.empty() && x.back() == 0u)
        {
            x.pop_back();
        }
        return true;
    }

    void BigInt::div_mod(const BigInt &a, const BigInt &b, BigInt &q, BigInt &r)
    {
        if (b.d.empty())
        {
            throw std::domain_error("division by zero");
        }
        if (a.d.empty())
        {
            q.d.clear();
            q.neg = false;
            r.d.clear();
            r.neg = false;
            return;
        }
        int cmp = cmp_abs(a, b);
        if (cmp < 0)
        {
            q.d.clear();
            q.neg = false;
            r = a;
            r.neg = false;
            return;
        }
        if (cmp == 0)
        {
            q.d.assign(1, 1u);
            q.neg = false;
            r.d.clear();
            r.neg = false;
            return;
        }

        auto trim_vec = [](std::vector<uint32_t> &v)
        {
            while (!v.empty() && v.back() == 0u)
            {
                v.pop_back();
            }
        };

        q.d.assign(a.d.size(), 0u);
        q.neg = false;
        r.d.clear();
        r.neg = false;
        r.d.reserve(a.d.size());

        size_t i = a.d.size();
        while (i > 0)
        {
            r.d.insert(r.d.begin(), 0u);
            r.d[0] = a.d[i - 1];
            while (!r.d.empty() && r.d.back() == 0u)
            {
                r.d.pop_back();
            }

            uint32_t low = 0;
            uint32_t high = BASE - 1;
            uint32_t best = 0;

            while (low <= high)
            {
                uint32_t mid = static_cast<uint32_t>(low + (static_cast<uint64_t>(high - low) / 2));
                std::vector<uint32_t> prod(b.d.size() + 1, 0u);
                uint64_t carry = 0;
                size_t j = 0;
                while (j < b.d.size())
                {
                    uint64_t cur = static_cast<uint64_t>(b.d[j]) * mid + carry;
                    prod[j] = static_cast<uint32_t>(cur % BASE);
                    carry = cur / BASE;
                    j += 1;
                }
                prod[b.d.size()] = static_cast<uint32_t>(carry);
                trim_vec(prod);

                std::vector<uint32_t> rr = r.d;
                bool ok = sub_inplace(rr, prod);
                if (ok)
                {
                    best = mid;
                    low = mid + 1;
                }
                else
                {
                    if (mid == 0)
                    {
                        break;
                    }
                    high = static_cast<uint32_t>(mid - 1);
                }
            }

            q.d[i - 1] = best;

            if (best > 0)
            {
                std::vector<uint32_t> prod(b.d.size() + 1, 0u);
                uint64_t carry = 0;
                size_t j = 0;
                while (j < b.d.size())
                {
                    uint64_t cur = static_cast<uint64_t>(b.d[j]) * best + carry;
                    prod[j] = static_cast<uint32_t>(cur % BASE);
                    carry = cur / BASE;
                    j += 1;
                }
                prod[b.d.size()] = static_cast<uint32_t>(carry);
                trim_vec(prod);
                sub_inplace(r.d, prod);
            }

            i -= 1;
        }

        while (!q.d.empty() && q.d.back() == 0u)
        {
            q.d.pop_back();
        }
        r.neg = false;
        q.neg = false;
        q.trim();
        r.trim();
    }

    BigInt &BigInt::operator+=(const BigInt &rhs)
    {
        if (neg == rhs.neg)
        {
            *this = add_abs(*this, rhs);
            neg = rhs.neg;
            return *this;
        }
        else
        {
            int c = cmp_abs(*this, rhs);
            if (c >= 0)
            {
                *this = sub_abs(*this, rhs);
                if (this->d.empty())
                {
                    neg = false;
                }
                return *this;
            }
            else
            {
                BigInt t = sub_abs(rhs, *this);
                t.neg = rhs.neg;
                *this = std::move(t);
                return *this;
            }
        }
    }

    BigInt &BigInt::operator-=(const BigInt &rhs)
    {
        if (neg != rhs.neg)
        {
            bool sign = neg;
            *this = add_abs(*this, rhs);
            neg = sign;
            return *this;
        }
        else
        {
            int c = cmp_abs(*this, rhs);
            if (c >= 0)
            {
                bool sign = neg;
                *this = sub_abs(*this, rhs);
                if (d.empty())
                {
                    neg = false;
                }
                else
                {
                    neg = sign;
                }
                return *this;
            }
            else
            {
                BigInt t = sub_abs(rhs, *this);
                t.neg = !neg;
                *this = std::move(t);
                return *this;
            }
        }
    }

    BigInt &BigInt::operator*=(const BigInt &rhs)
    {
        bool sign = false;
        if (neg != rhs.neg)
        {
            sign = true;
        }
        BigInt r = mul_abs(*this, rhs);
        r.neg = false;
        if (r.d.empty())
        {
            r.neg = false;
        }
        else
        {
            if (sign)
            {
                r.neg = true;
            }
        }
        *this = std::move(r);
        return *this;
    }

    BigInt &BigInt::operator/=(const BigInt &rhs)
    {
        if (rhs.d.empty())
        {
            throw std::domain_error("division by zero");
        }
        BigInt qa;
        BigInt ra;
        BigInt aa = *this;
        aa.neg = false;
        BigInt bb = rhs;
        bb.neg = false;
        div_mod(aa, bb, qa, ra);
        if (!qa.d.empty())
        {
            qa.neg = neg != rhs.neg;
        }
        else
        {
            qa.neg = false;
        }
        *this = std::move(qa);
        return *this;
    }

    BigInt operator+(BigInt lhs, const BigInt &rhs)
    {
        lhs += rhs;
        return lhs;
    }

    BigInt operator-(BigInt lhs, const BigInt &rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    BigInt operator*(BigInt lhs, const BigInt &rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    BigInt operator/(BigInt lhs, const BigInt &rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    bool operator==(const BigInt &a, const BigInt &b)
    {
        if (a.neg != b.neg)
        {
            if (a.d.empty() && b.d.empty())
            {
                return true;
            }
            return false;
        }
        if (a.d.size() != b.d.size())
        {
            return false;
        }
        size_t i = 0;
        while (i < a.d.size())
        {
            if (a.d[i] != b.d[i])
            {
                return false;
            }
            i += 1;
        }
        return true;
    }

    bool operator!=(const BigInt &a, const BigInt &b)
    {
        return !(a == b);
    }

    bool operator<(const BigInt &a, const BigInt &b)
    {
        if (a.neg != b.neg)
        {
            if (a.neg)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        int c = BigInt::cmp_abs(a, b);
        if (!a.neg)
        {
            if (c < 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (c > 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    bool operator>(const BigInt &a, const BigInt &b)
    {
        return b < a;
    }

    bool operator<=(const BigInt &a, const BigInt &b)
    {
        return !(b < a);
    }

    bool operator>=(const BigInt &a, const BigInt &b)
    {
        return !(a < b);
    }

    std::ostream &operator<<(std::ostream &os, const BigInt &v)
    {
        if (v.d.empty())
        {
            os << "0";
            return os;
        }
        if (v.neg)
        {
            os << "-";
        }
        os << v.d.back();
        size_t i = v.d.size();
        while (i > 1)
        {
            uint32_t block = v.d[i - 2];
            os << std::setw(BigInt::BASE_DIGS) << std::setfill('0') << block;
            i -= 1;
        }
        return os;
    }

    std::istream &operator>>(std::istream &is, BigInt &v)
    {
        std::string s;
        if (!(is >> s))
        {
            return is;
        }
        v = BigInt::from_string(s);
        return is;
    }

}
