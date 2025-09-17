#pragma once

#include <initializer_list>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <iterator>

#include "my_container/deque.hpp"

namespace my_container
{

    template <typename T, typename Container = Deque<T>>
    class Stack
    {
    public:
        using value_type = T;
        using container_type = Container;
        using size_type = typename container_type::size_type;
        using reference = value_type &;
        using const_reference = const value_type &;

        Stack() = default;

        explicit Stack(const container_type &cont)
            : c_(cont)
        {
        }

        explicit Stack(container_type &&cont) noexcept(std::is_nothrow_move_constructible<container_type>::value)
            : c_(std::move(cont))
        {
        }

        explicit Stack(std::initializer_list<value_type> init)
        {
            std::copy(init.begin(), init.end(), std::back_inserter(c_));
        }

        Stack(const Stack &) = default;
        Stack(Stack &&) noexcept(std::is_nothrow_move_constructible<container_type>::value) = default;

        ~Stack() = default;

        Stack &operator=(const Stack &) = default;
        Stack &operator=(Stack &&) noexcept(std::is_nothrow_move_assignable<container_type>::value) = default;

        reference top()
        {
            if (c_.empty())
                throw std::out_of_range("Stack::top on empty stack");
            return c_.back();
        }

        const_reference top() const
        {
            if (c_.empty())
                throw std::out_of_range("Stack::top on empty stack");
            return c_.back();
        }

        bool empty() const noexcept
        {
            return c_.empty();
        }
        size_type size() const noexcept
        {
            return c_.size();
        }

        void push(const value_type &value) { c_.push_back(value); }
        void push(value_type &&value) { c_.push_back(std::move(value)); }

        void pop()
        {
            if (c_.empty())
                throw std::out_of_range("Stack::pop on empty stack");
            c_.pop_back();
        }

        void swap(Stack &other) noexcept(noexcept(std::declval<container_type &>().swap(std::declval<container_type &>())))
        {
            c_.swap(other.c_);
        }

        friend bool operator==(const Stack &a, const Stack &b) { return a.c_ == b.c_; }
        friend bool operator!=(const Stack &a, const Stack &b) { return !(a == b); }
        friend bool operator<(const Stack &a, const Stack &b) { return a.c_ < b.c_; }
        friend bool operator>(const Stack &a, const Stack &b) { return b < a; }
        friend bool operator<=(const Stack &a, const Stack &b) { return !(b < a); }
        friend bool operator>=(const Stack &a, const Stack &b) { return !(a < b); }

#if (__cplusplus >= 202002L) && (defined(__cpp_lib_three_way_comparison) || defined(__cpp_impl_three_way_comparison))
        friend auto operator<=>(const Stack &a, const Stack &b) = default;
#endif

        const container_type &container() const noexcept { return c_; }

    private:
        container_type c_;
    };

    template <typename T, typename Container>
    inline void swap(Stack<T, Container> &x, Stack<T, Container> &y) noexcept(noexcept(x.swap(y)))
    {
        x.swap(y);
    }

}
