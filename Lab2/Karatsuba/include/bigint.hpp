#pragma once
#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

namespace core
{

    class BigInt
    {
    public:
        BigInt();
        explicit BigInt(long long v);
        explicit BigInt(const std::string &s);
        BigInt(const BigInt &other);
        BigInt(BigInt &&other) noexcept;
        BigInt &operator=(const BigInt &other);
        BigInt &operator=(BigInt &&other) noexcept;

        BigInt &operator+=(const BigInt &rhs);
        BigInt &operator-=(const BigInt &rhs);
        BigInt &operator*=(const BigInt &rhs);
        BigInt &operator/=(const BigInt &rhs);

        friend BigInt operator+(BigInt lhs, const BigInt &rhs);
        friend BigInt operator-(BigInt lhs, const BigInt &rhs);
        friend BigInt operator*(BigInt lhs, const BigInt &rhs);
        friend BigInt operator/(BigInt lhs, const BigInt &rhs);

        friend bool operator==(const BigInt &a, const BigInt &b);
        friend bool operator!=(const BigInt &a, const BigInt &b);
        friend bool operator<(const BigInt &a, const BigInt &b);
        friend bool operator>(const BigInt &a, const BigInt &b);
        friend bool operator<=(const BigInt &a, const BigInt &b);
        friend bool operator>=(const BigInt &a, const BigInt &b);

        friend std::ostream &operator<<(std::ostream &os, const BigInt &v);
        friend std::istream &operator>>(std::istream &is, BigInt &v);

        static BigInt from_string(const std::string &s);
        std::string to_string() const;

    private:
        static const uint32_t BASE = 1000000000u;
        static const uint32_t BASE_DIGS = 9u;

        // Karatsuba

        static const size_t KARATSUBA_THRESHOLD = 32;

        std::vector<uint32_t> d;
        bool neg;

        void trim();
        static int cmp_abs(const BigInt &a, const BigInt &b);
        static BigInt add_abs(const BigInt &a, const BigInt &b);
        static BigInt sub_abs(const BigInt &a, const BigInt &b);
        static BigInt mul_abs(const BigInt &a, const BigInt &b);
        static void div_mod(const BigInt &a, const BigInt &b, BigInt &q, BigInt &r);

        static void add_inplace(std::vector<uint32_t> &x, const std::vector<uint32_t> &y);
        static bool sub_inplace(std::vector<uint32_t> &x, const std::vector<uint32_t> &y);

        static BigInt mul_karatsuba_abs(const BigInt &a, const BigInt &b);
        static void split_at(const BigInt &a, size_t k, BigInt &low, BigInt &high);
        static BigInt shift_base_abs(const BigInt &a, size_t k);
    };

}
