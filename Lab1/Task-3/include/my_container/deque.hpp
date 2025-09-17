#pragma once

#include <initializer_list>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <compare>

#include "my_container/list.hpp"

namespace my_container
{

    template <typename T>
    class Deque final : public List<T>
    {
    private:
        using base = List<T>;

    public:
        using value_type = typename base::value_type;
        using reference = typename base::reference;
        using const_reference = typename base::const_reference;
        using pointer = typename base::pointer;
        using const_pointer = typename base::const_pointer;
        using size_type = typename base::size_type;
        using difference_type = typename base::difference_type;

        using iterator = typename base::iterator;
        using const_iterator = typename base::const_iterator;
        using reverse_iterator = typename base::reverse_iterator;
        using const_reverse_iterator = typename base::const_reverse_iterator;

        Deque() : base() {}

        explicit Deque(std::initializer_list<T> ilist) : base(ilist) {}

        Deque(const Deque &other) : base(static_cast<const base &>(other)) {}

        Deque(Deque &&other) noexcept : base(std::move(static_cast<base &>(other))) {}

        Deque &operator=(const Deque &other)
        {
            if (this != &other)
            {
                base::operator=(static_cast<const base &>(other));
            }
            return *this;
        }

        Deque &operator=(Deque &&other) noexcept
        {
            if (this != &other)
            {
                base::operator=(std::move(static_cast<base &>(other)));
            }
            return *this;
        }

        ~Deque() override = default;

        using base::back;
        using base::front;

        reference at(size_type index)
        {
            return const_cast<reference>(static_cast<const Deque &>(*this).at(index));
        }
        const_reference at(size_type index) const
        {
            const size_type n = this->size();
            if (index >= n)
                throw std::out_of_range("Deque::at index out of range");
            if (index <= n / 2)
            {
                auto it = this->begin();
                for (size_type i = 0; i < index; ++i)
                    ++it;
                return *it;
            }
            else
            {
                auto it = this->end();
                for (size_type steps = n - index; steps > 0; --steps)
                    --it;
                return *it;
            }
        }

        reference operator[](size_type index) { return at(index); }
        const_reference operator[](size_type index) const { return at(index); }

        using base::begin;
        using base::cbegin;
        using base::cend;
        using base::crbegin;
        using base::crend;
        using base::end;
        using base::rbegin;
        using base::rend;

        using base::empty;
        using base::max_size;
        using base::size;

        using base::clear;
        using base::erase;
        using base::insert;
        using base::pop_back;
        using base::pop_front;
        using base::push_back;
        using base::push_front;
        using base::resize;
        using base::swap;

        iterator insert(iterator pos, const T &value)
        {
            return base::insert(const_iterator(pos), value);
        }
        iterator insert(iterator pos, T &&value)
        {
            return base::insert(const_iterator(pos), std::move(value));
        }
        iterator erase(iterator pos)
        {
            return base::erase(const_iterator(pos));
        }

        friend bool operator==(const Deque &a, const Deque &b) noexcept(noexcept(std::declval<const T &>() == std::declval<const T &>()))
        {
            if (a.size() != b.size())
                return false;
            return std::equal(a.begin(), a.end(), b.begin());
        }
        friend bool operator!=(const Deque &a, const Deque &b) noexcept(noexcept(a == b))
        {
            return !(a == b);
        }
        friend bool operator<(const Deque &a, const Deque &b)
        {
            return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
        }
        friend bool operator>(const Deque &a, const Deque &b)
        {
            return b < a;
        }
        friend bool operator<=(const Deque &a, const Deque &b)
        {
            return !(b < a);
        }
        friend bool operator>=(const Deque &a, const Deque &b)
        {
            return !(a < b);
        }

        friend auto operator<=>(const Deque &a, const Deque &b)
        {
            using std::compare_three_way;
            using std::lexicographical_compare_three_way;
            return lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end(), compare_three_way());
        }
    };

}
