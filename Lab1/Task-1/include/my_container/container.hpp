#pragma once

#include <cstddef>

namespace my_container
{

    template <typename T>
    class Container
    {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        using iterator = pointer;
        using const_iterator = const_pointer;

        virtual ~Container() noexcept = default;

        virtual iterator begin() noexcept = 0;
        virtual const_iterator begin() const noexcept = 0;
        virtual iterator end() noexcept = 0;
        virtual const_iterator end() const noexcept = 0;

        const_iterator cbegin() const noexcept
        {
            return begin();
        }

        const_iterator cend() const noexcept
        {
            return end();
        }

        virtual size_type size() const noexcept = 0;
        virtual size_type max_size() const noexcept = 0;
        virtual bool empty() const noexcept = 0;

        bool operator==(const Container &other) const noexcept
        {
            return equals(other);
        }

        bool operator!=(const Container &other) const noexcept
        {
            return !(*this == other);
        }

    protected:
        Container() noexcept = default;
        
        Container(const Container &) = default;
        Container &operator=(const Container &) = default;

        virtual bool equals(const Container &other) const noexcept = 0;
    };

}
