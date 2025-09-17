#pragma once

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <cstddef>

#include <utility>

#include "my_container/container.hpp"

namespace my_container
{

    template <typename T, std::size_t N>
    class Array final : public Container<T>
    {
        static_assert(N > 0, "size must be > 0");

    public:
        using Base = Container<T>;
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using difference_type = typename Base::difference_type;
        using reference = typename Base::reference;
        using const_reference = typename Base::const_reference;
        using pointer = typename Base::pointer;
        using const_pointer = typename Base::const_pointer;
        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        Array() noexcept(std::is_nothrow_default_constructible<T>::value) = default;

        Array(const Array &other) = default;

        Array(Array &&other) noexcept(std::is_nothrow_move_constructible<T>::value) = default;

        explicit Array(std::initializer_list<T> list)
        {
            if (list.size() != N)
            {
                throw std::length_error("wrong count elements in initializer_list");
            }
            std::copy(list.begin(), list.end(), _data);
        }

        ~Array() override = default;

        Array &operator=(const Array &other) = default;

        Array &operator=(Array &&other) noexcept(std::is_nothrow_move_assignable<T>::value) = default;

        // cppcheck-suppress functionConst
        reference at(size_type pos)
        {
            if (pos >= N)
            {
                throw std::out_of_range("out of range");
            }
            return _data[pos];
        }

        const_reference at(size_type pos) const
        {
            if (pos >= N)
            {
                throw std::out_of_range("out of range");
            }
            return _data[pos];
        }

        // cppcheck-suppress functionConst
        reference operator[](size_type pos) noexcept
        {
            return _data[pos];
        }

        const_reference operator[](size_type pos) const noexcept
        {
            return _data[pos];
        }

        // cppcheck-suppress functionConst
        reference front() noexcept
        {
            return _data[0];
        }

        const_reference front() const noexcept
        {
            return _data[0];
        }

        // cppcheck-suppress functionConst
        reference back() noexcept
        {
            return _data[N - 1];
        }

        const_reference back() const noexcept
        {
            return _data[N - 1];
        }

        // cppcheck-suppress functionConst
        pointer data() noexcept
        {
            return _data;
        }

        const_pointer data() const noexcept
        {
            return _data;
        }

        iterator begin() noexcept override
        {
            return _data;
        }

        const_iterator begin() const noexcept override
        {
            return _data;
        }

        iterator end() noexcept override
        {
            return _data + N;
        }

        const_iterator end() const noexcept override
        {
            return _data + N;
        }

        const_iterator cbegin() const noexcept
        {
            return begin();
        }

        const_iterator cend() const noexcept
        {
            return end();
        }

        reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(end());
        }

        const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }

        const_reverse_iterator crbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }

        reverse_iterator rend() noexcept
        {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        size_type size() const noexcept override
        {
            return N;
        }

        size_type max_size() const noexcept override
        {
            return N;
        }

        bool empty() const noexcept override
        {
            return N == 0;
        }

        void fill(const T &value)
        {
            std::fill(begin(), end(), value);
        }

        void swap(Array &other) noexcept(noexcept(std::swap(std::declval<T &>(), std::declval<T &>())))
        {
            using std::swap;
            for (size_type i = 0; i < N; ++i)
            {
                swap(_data[i], other._data[i]);
            }
        }

        bool operator==(const Array &rhs) const noexcept
        {
            return std::equal(begin(), end(), rhs.begin(), rhs.end());
        }

        bool operator!=(const Array &rhs) const noexcept
        {
            return !(*this == rhs);
        }

        bool operator<(const Array &rhs) const noexcept
        {
            return std::lexicographical_compare(begin(), end(), rhs.begin(), rhs.end());
        }

        bool operator<=(const Array &rhs) const noexcept
        {
            return !(rhs < *this);
        }

        bool operator>(const Array &rhs) const noexcept
        {
            return rhs < *this;
        }

        bool operator>=(const Array &rhs) const noexcept
        {
            return !(*this < rhs);
        }

        int compare(const Array &rhs) const noexcept
        {
            for (size_type i = 0; i < N; ++i)
            {
                if (_data[i] < rhs._data[i])
                {
                    return -1;
                }
                if (rhs._data[i] < _data[i])
                {
                    return 1;
                }
            }
            return 0;
        }

    protected:
        bool equals(const Base &other) const noexcept override
        {
            auto ptr = dynamic_cast<const Array *>(&other);
            return ptr && (*this == *ptr);
        }

    private:
        T _data[N] = {};
    };

    template <typename T>
    class Array<T, 0> final : public Container<T>
    {
    public:
        using Base = Container<T>;
        using value_type = typename Base::value_type;
        using size_type = typename Base::size_type;
        using difference_type = typename Base::difference_type;
        using reference = typename Base::reference;
        using const_reference = typename Base::const_reference;
        using pointer = typename Base::pointer;
        using const_pointer = typename Base::const_pointer;
        using iterator = typename Base::iterator;
        using const_iterator = typename Base::const_iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        Array() noexcept = default;

        Array(const Array &other) = default;

        Array(Array &&other) noexcept = default;

        explicit Array(std::initializer_list<T> list)
        {
            if (list.size() != 0)
            {
                throw std::length_error("initializer_list must be em N == 0");
            }
        }

        ~Array() override = default;

        Array &operator=(const Array &other) = default;

        Array &operator=(Array &&other) noexcept = default;

        reference at(size_type)
        {
            throw std::out_of_range("Array<...,0>::at");
        }

        const_reference at(size_type) const
        {
            throw std::out_of_range("Array<...,0>::at");
        }

        reference operator[](size_type) noexcept
        {
            __builtin_unreachable();
        }

        const_reference operator[](size_type) const noexcept
        {
            __builtin_unreachable();
        }

        pointer data() noexcept
        {
            return nullptr;
        }

        const_pointer data() const noexcept
        {
            return nullptr;
        }

        iterator begin() noexcept override
        {
            return nullptr;
        }

        const_iterator begin() const noexcept override
        {
            return nullptr;
        }

        iterator end() noexcept override
        {
            return nullptr;
        }

        const_iterator end() const noexcept override
        {
            return nullptr;
        }

        const_iterator cbegin() const noexcept
        {
            return begin();
        }

        const_iterator cend() const noexcept
        {
            return end();
        }

        reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(end());
        }

        const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }

        const_reverse_iterator crbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }

        reverse_iterator rend() noexcept
        {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        size_type size() const noexcept override
        {
            return 0;
        }

        size_type max_size() const noexcept override
        {
            return 0;
        }

        bool empty() const noexcept override
        {
            return true;
        }

        void fill(const T &) noexcept
        {
        }

        void swap(Array &) noexcept
        {
        }

        bool operator==(const Array &) const noexcept
        {
            return true;
        }

        bool operator!=(const Array &) const noexcept
        {
            return false;
        }

        bool operator<(const Array &) const noexcept
        {
            return false;
        }

        bool operator<=(const Array &) const noexcept
        {
            return true;
        }

        bool operator>(const Array &) const noexcept
        {
            return false;
        }

        bool operator>=(const Array &) const noexcept
        {
            return true;
        }

        int compare(const Array &) const noexcept
        {
            return 0;
        }

    protected:
        bool equals(const Base &other) const noexcept override
        {
            return dynamic_cast<const Array *>(&other) != nullptr;
        }
    };

}
