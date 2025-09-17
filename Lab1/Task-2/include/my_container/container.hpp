#pragma once

#include <cstddef>

namespace my_container
{

    template <typename T, typename Iter, typename ConstIter>
    class Container
    {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using iterator = Iter;
        using const_iterator = ConstIter;

        virtual ~Container() noexcept = default;

        virtual iterator begin() noexcept = 0;
        virtual const_iterator begin() const noexcept = 0;
        virtual const_iterator cbegin() const noexcept = 0;

        virtual iterator end() noexcept = 0;
        virtual const_iterator end() const noexcept = 0;
        virtual const_iterator cend() const noexcept = 0;

        virtual bool operator==(const Container &rhs) const noexcept = 0;
        virtual bool operator!=(const Container &rhs) const noexcept = 0;

        virtual size_type size() const noexcept = 0;
        virtual size_type max_size() const noexcept = 0;
        virtual bool empty() const noexcept = 0;

    protected:
        Container() = default;
        Container(const Container &) = default;
        Container &operator=(const Container &) = default;
    };

}
