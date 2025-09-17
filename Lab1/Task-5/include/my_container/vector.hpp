#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <compare>

#include "my_container/container.hpp"

namespace my_container
{

    template <typename T, typename Alloc = std::allocator<T>>
    class Vector final : public Container<T, T *, const T *>
    {
    public:
        using value_type = T;
        using reference = T &;
        using const_reference = const T &;
        using pointer = T *;
        using const_pointer = const T *;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using iterator = T *;
        using const_iterator = const T *;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    private:
        pointer data_{nullptr};
        size_type size_{0};
        size_type cap_{0};

        Alloc alloc_;

        static size_type max_cap_possible() noexcept
        {
            const size_type m = std::numeric_limits<size_type>::max() / sizeof(T);
            if (m > 1)
            {
                return m - 1;
            }
            else
            {
                return 0;
            }
        }

        pointer allocate_raw(size_type n)
        {
            if (n == 0)
                return nullptr;
            if (n > max_cap_possible())
            {
                throw std::length_error("Vector capacity too large");
            }
            return std::allocator_traits<Alloc>::allocate(alloc_, n);
        }

        void deallocate_raw(pointer p, size_type n) noexcept
        {
            if (!p || n == 0)
                return;
            std::allocator_traits<Alloc>::deallocate(alloc_, p, n);
        }

        static void destroy_range(Alloc &a, pointer first, const pointer last) noexcept
        {
            for (; first != last; ++first)
            {
                std::allocator_traits<Alloc>::destroy(a, std::addressof(*first));
            }
        }

        static void uninitialized_move_n(Alloc &a, pointer src, size_type count, pointer dest)
        {
            size_type i = 0;
            try
            {
                for (; i < count; ++i)
                {
                    std::allocator_traits<Alloc>::construct(
                        a, std::addressof(dest[i]), std::move_if_noexcept(src[i]));
                }
            }
            catch (...)
            {
                destroy_range(a, dest, dest + i);
                throw;
            }
        }

        static void uninitialized_copy_n(Alloc &a, const_pointer src, size_type count, pointer dest)
        {
            size_type i = 0;
            try
            {
                for (; i < count; ++i)
                {
                    std::allocator_traits<Alloc>::construct(
                        a, std::addressof(dest[i]), src[i]);
                }
            }
            catch (...)
            {
                destroy_range(a, dest, dest + i);
                throw;
            }
        }

        void reallocate_to(size_type new_cap)
        {
            pointer new_data = allocate_raw(new_cap);
            try
            {
                uninitialized_move_n(alloc_, data_, size_, new_data);
            }
            catch (...)
            {
                deallocate_raw(new_data, new_cap);
                throw;
            }
            destroy_range(alloc_, data_, data_ + size_);
            deallocate_raw(data_, cap_);
            data_ = new_data;
            cap_ = new_cap;
        }

        bool owns_iterator(const_iterator it) const noexcept
        {
            return (it >= data_) && (it <= data_ + size_);
        }

        void ensure_grow_for(size_type add)
        {
            if (size_ + add <= cap_)
            {
                return;
            }

            size_type new_cap = 0;

            if (cap_ == 0)
            {
                new_cap = (add > 1 ? add : 1);
            }
            else
            {
                size_type grown = cap_ + (cap_ >> 1) + 1;

                if (grown > size_ + add)
                {
                    new_cap = grown;
                }
                else
                {
                    new_cap = size_ + add;
                }
            }

            reallocate_to(new_cap);
        }

        template <typename U>
        iterator insert_impl(const_iterator pos, U &&value)
        {
            if (!owns_iterator(pos))
            {
                throw std::out_of_range("Vector::insert: invalid iterator");
            }

            const size_type idx = static_cast<size_type>(pos - cbegin());

            if (size_ == cap_)
            {
                // перераспределение
                size_type new_cap;
                if (cap_ == 0)
                {
                    new_cap = 1;
                }
                else
                {
                    new_cap = cap_ + (cap_ >> 1) + 1;
                }

                if (new_cap < size_ + 1)
                    new_cap = size_ + 1;

                pointer new_data = allocate_raw(new_cap);
                size_type i = 0;
                try
                {
                    // до pos
                    for (; i < idx; ++i)
                    {
                        std::allocator_traits<Alloc>::construct(
                            alloc_, std::addressof(new_data[i]),
                            std::move_if_noexcept(data_[i]));
                    }
                    // вставка
                    std::allocator_traits<Alloc>::construct(
                        alloc_, std::addressof(new_data[idx]), std::forward<U>(value));
                    // хвост
                    for (size_type j = idx; j < size_; ++j, ++i)
                    {
                        std::allocator_traits<Alloc>::construct(
                            alloc_, std::addressof(new_data[i + 1]),
                            std::move_if_noexcept(data_[j]));
                    }
                }
                catch (...)
                {
                    destroy_range(alloc_, new_data, new_data + (i + 1));
                    deallocate_raw(new_data, new_cap);
                    throw;
                }

                destroy_range(alloc_, data_, data_ + size_);
                deallocate_raw(data_, cap_);
                data_ = new_data;
                cap_ = new_cap;
                size_ += 1;
                return data_ + idx;
            }
            else
            {
                if (idx == size_)
                {
                    std::allocator_traits<Alloc>::construct(
                        alloc_, std::addressof(data_[size_]), std::forward<U>(value));
                    ++size_;
                    return data_ + (size_ - 1);
                }

                std::allocator_traits<Alloc>::construct(
                    alloc_, std::addressof(data_[size_]),
                    std::move_if_noexcept(data_[size_ - 1]));

                for (size_type i = size_ - 1; i > idx; --i)
                {
                    data_[i] = std::move_if_noexcept(data_[i - 1]);
                }
                data_[idx] = std::forward<U>(value);
                ++size_;
                return data_ + idx;
            }
        }

    public:
        Vector() noexcept = default;

        explicit Vector(std::initializer_list<T> list)
        {
            const size_type n = static_cast<size_type>(list.size());
            if (n > 0)
            {
                cap_ = n;
                data_ = allocate_raw(cap_);
                uninitialized_copy_n(alloc_, list.begin(), n, data_);
                size_ = n;
            }
        }

        Vector(const Vector &other)
        {
            if (other.size_ > 0)
            {
                cap_ = other.size_;
                data_ = allocate_raw(cap_);
                uninitialized_copy_n(alloc_, other.data_, other.size_, data_);
                size_ = other.size_;
            }
        }

        Vector(Vector &&other) noexcept
            : data_(other.data_), size_(other.size_), cap_(other.cap_), alloc_(std::move(other.alloc_))
        {
            other.data_ = nullptr;
            other.size_ = 0;
            other.cap_ = 0;
        }

        ~Vector() override
        {
            clear();
            deallocate_raw(data_, cap_);
        }

        Vector &operator=(const Vector &other)
        {
            if (this != &other)
            {
                Vector tmp(other);
                swap(tmp);
            }
            return *this;
        }

        Vector &operator=(Vector &&other) noexcept
        {
            if (this != &other)
            {
                clear();
                deallocate_raw(data_, cap_);
                data_ = other.data_;
                size_ = other.size_;
                cap_ = other.cap_;
                other.data_ = nullptr;
                other.size_ = 0;
                other.cap_ = 0;
            }
            return *this;
        }

        Vector &operator=(std::initializer_list<T> list)
        {
            Vector tmp(list);
            swap(tmp);
            return *this;
        }

        reference operator[](size_type idx) noexcept { return data_[idx]; }
        const_reference operator[](size_type idx) const noexcept { return data_[idx]; }

        reference at(size_type idx)
        {
            if (idx >= size_)
                throw std::out_of_range("Vector::at");
            return data_[idx];
        }
        const_reference at(size_type idx) const
        {
            if (idx >= size_)
                throw std::out_of_range("Vector::at");
            return data_[idx];
        }

        reference front()
        {
            if (empty())
                throw std::out_of_range("Vector::front");
            return data_[0];
        }
        const_reference front() const
        {
            if (empty())
                throw std::out_of_range("Vector::front");
            return data_[0];
        }

        reference back()
        {
            if (empty())
                throw std::out_of_range("Vector::back");
            return data_[size_ - 1];
        }
        const_reference back() const
        {
            if (empty())
                throw std::out_of_range("Vector::back");
            return data_[size_ - 1];
        }

        pointer data() noexcept { return data_; }
        const_pointer data() const noexcept { return data_; }

        iterator begin() noexcept override { return data_; }
        const_iterator begin() const noexcept override { return data_; }
        const_iterator cbegin() const noexcept override { return data_; }

        iterator end() noexcept override { return data_ + size_; }
        const_iterator end() const noexcept override { return data_ + size_; }
        const_iterator cend() const noexcept override { return data_ + size_; }

        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
        const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

        bool empty() const noexcept override { return size_ == 0; }
        size_type size() const noexcept override { return size_; }
        size_type capacity() const noexcept { return cap_; }

        size_type max_size() const noexcept override
        {
            return max_cap_possible();
        }

        void reserve(size_type new_cap)
        {
            if (new_cap > cap_)
            {
                reallocate_to(new_cap);
            }
        }

        void shrink_to_fit()
        {
            if (size_ < cap_)
            {
                reallocate_to(size_);
            }
        }

        void clear() noexcept
        {
            destroy_range(alloc_, data_, data_ + size_);
            size_ = 0;
        }

        void push_back(const T &value)
        {
            ensure_grow_for(1);
            std::allocator_traits<Alloc>::construct(alloc_, std::addressof(data_[size_]), value);
            ++size_;
        }

        void push_back(T &&value)
        {
            ensure_grow_for(1);
            std::allocator_traits<Alloc>::construct(alloc_, std::addressof(data_[size_]), std::move(value));
            ++size_;
        }

        void pop_back()
        {
            if (empty())
                throw std::out_of_range("Vector::pop_back");
            --size_;
            std::allocator_traits<Alloc>::destroy(alloc_, std::addressof(data_[size_]));
        }

        iterator insert(const_iterator pos, const T &value)
        {
            return insert_impl(pos, value);
        }

        iterator insert(const_iterator pos, T &&value)
        {
            return insert_impl(pos, std::move(value));
        }

        iterator erase(const_iterator pos)
        {
            if (!owns_iterator(pos) || pos == end())
            {
                throw std::out_of_range("Vector::erase");
            }
            const size_type idx = static_cast<size_type>(pos - cbegin());

            std::allocator_traits<Alloc>::destroy(alloc_, std::addressof(data_[idx]));
            for (size_type i = idx; i + 1 < size_; ++i)
            {
                data_[i] = std::move_if_noexcept(data_[i + 1]);
            }
            --size_;
            return data_ + idx;
        }

        void resize(size_type count)
        {
            if (count < size_)
            {
                destroy_range(alloc_, data_ + count, data_ + size_);
                size_ = count;
            }
            else if (count > size_)
            {
                ensure_grow_for(count - size_);
                size_type i = size_;
                for (; i < count; ++i)
                {
                    std::allocator_traits<Alloc>::construct(alloc_, std::addressof(data_[i]));
                }
                size_ = count;
            }
        }

        void swap(Vector &other) noexcept
        {
            using std::swap;
            swap(data_, other.data_);
            swap(size_, other.size_);
            swap(cap_, other.cap_);
        }

        bool operator==(const Container<T, iterator, const_iterator> &rhs) const noexcept override
        {
            const auto &r = static_cast<const Vector &>(rhs);
            if (size_ != r.size_)
                return false;
            for (size_type i = 0; i < size_; ++i)
            {
                if (!(data_[i] == r.data_[i]))
                    return false;
            }
            return true;
        }

        bool operator!=(const Container<T, iterator, const_iterator> &rhs) const noexcept override
        {
            return !(*this == rhs);
        }

        friend bool operator<(const Vector &a, const Vector &b)
        {
            return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
        }

        friend bool operator>(const Vector &a, const Vector &b)
        {
            return b < a;
        }

        friend bool operator<=(const Vector &a, const Vector &b)
        {
            return !(b < a);
        }

        friend bool operator>=(const Vector &a, const Vector &b)
        {
            return !(a < b);
        }

        friend bool operator==(const Vector &a, const Vector &b)
        {
            if (a.size_ != b.size_)
                return false;
            for (size_type i = 0; i < a.size_; ++i)
            {
                if (!(a.data_[i] == b.data_[i]))
                    return false;
            }
            return true;
        }

        friend bool operator!=(const Vector &a, const Vector &b)
        {
            return !(a == b);
        }

        friend std::strong_ordering operator<=>(const Vector &a, const Vector &b)
        {
            const size_type n = std::min(a.size_, b.size_);
            for (size_type i = 0; i < n; ++i)
            {
                auto cmp = a.data_[i] <=> b.data_[i];
                if (cmp != 0)
                    return cmp;
            }
            if (a.size_ < b.size_)
                return std::strong_ordering::less;
            if (a.size_ > b.size_)
                return std::strong_ordering::greater;
            return std::strong_ordering::equivalent;
        }
    };

}
