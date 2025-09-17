#pragma once

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

#include "my_container/container.hpp"

namespace my_container
{

    template <typename T>
    struct ListNode
    {
        ListNode *prev;
        ListNode *next;
        T value;

        explicit ListNode(const T &v) : prev(nullptr), next(nullptr), value(v)
        {
        }

        explicit ListNode(T &&v)
            : prev(nullptr), next(nullptr), value(std::move(v))
        {
        }

        ListNode()
            : prev(this), next(this), value()
        {
        }
    };

    template <typename T>
    class ListIterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        ListIterator()
            : _p(nullptr)
        {
        }

        explicit ListIterator(ListNode<T> *p)
            : _p(p)
        {
        }

        reference operator*() const
        {
            return _p->value;
        }
        pointer operator->() const
        {
            return &_p->value;
        }

        ListIterator &operator++()
        {
            _p = _p->next;
            return *this;
        }

        ListIterator operator++(int)
        {
            ListIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        ListIterator &operator--()
        {
            _p = _p->prev;
            return *this;
        }

        ListIterator operator--(int)
        {
            ListIterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const ListIterator &rhs) const
        {
            return _p == rhs._p;
        }
        bool operator!=(const ListIterator &rhs) const
        {
            return _p != rhs._p;
        }

        ListNode<T> *raw() const { return _p; }

    private:
        ListNode<T> *_p;
    };

    template <typename T>
    class ListConstIterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T *;
        using reference = const T &;

        ListConstIterator()
            : _p(nullptr)
        {
        }

        explicit ListConstIterator(const ListNode<T> *p)
            : _p(p)
        {
        }

        explicit ListConstIterator(const ListIterator<T> &it)
            : _p(it.raw())
        {
        }

        reference operator*() const
        {
            return _p->value;
        }
        pointer operator->() const
        {
            return &_p->value;
        }

        ListConstIterator &operator++()
        {
            _p = _p->next;
            return *this;
        }

        ListConstIterator operator++(int)
        {
            ListConstIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        ListConstIterator &operator--()
        {
            _p = _p->prev;
            return *this;
        }

        ListConstIterator operator--(int)
        {
            ListConstIterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const ListConstIterator &rhs) const
        {
            return _p == rhs._p;
        }
        bool operator!=(const ListConstIterator &rhs) const
        {
            return _p != rhs._p;
        }

        const ListNode<T> *raw() const
        {
            return _p;
        }

    private:
        const ListNode<T> *_p;
    };

    template <typename T>
    class List final
        : public Container<T, ListIterator<T>, ListConstIterator<T>>
    {
    private:
        using Node = ListNode<T>;

        Node _sentinel;
        std::size_t _size;

        static void link_before(Node *pos, Node *node)
        {
            node->next = pos;
            node->prev = pos->prev;
            pos->prev->next = node;
            pos->prev = node;
        }

        static void unlink(Node *node)
        {
            node->prev->next = node->next;
            node->next->prev = node->prev;
            node->next = nullptr;
            node->prev = nullptr;
        }

        template <typename U>
        ListIterator<T> emplace_before(Node *pos, U &&value)
        {
            Node *node = new Node(std::forward<U>(value));
            link_before(pos, node);
            ++_size;
            return ListIterator<T>(node);
        }

    public:
        using value_type = T;
        using reference = T &;
        using const_reference = const T &;
        using pointer = T *;
        using const_pointer = const T *;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using iterator = ListIterator<T>;
        using const_iterator = ListConstIterator<T>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        List() : _sentinel(), _size(0)
        {
        }

        explicit List(std::initializer_list<T> list) : List()
        {
            for (const auto &v : list)
            {
                push_back(v);
            }
        }

        List(const List &other) : List()
        {
            for (const auto &v : other)
            {
                push_back(v);
            }
        }

        List(List &&other) noexcept : List()
        {
            if (!other.empty())
            {
                _sentinel.next = other._sentinel.next;
                _sentinel.prev = other._sentinel.prev;
                _sentinel.next->prev = &_sentinel;
                _sentinel.prev->next = &_sentinel;
                _size = other._size;

                other._sentinel.next = &other._sentinel;
                other._sentinel.prev = &other._sentinel;
                other._size = 0;
            }
        }

        List &operator=(const List &other)
        {
            if (this == &other)
            {
                return *this;
            }
            List tmp(other);
            swap(tmp);
            return *this;
        }

        List &operator=(List &&other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }
            clear();
            if (!other.empty())
            {
                _sentinel.next = other._sentinel.next;
                _sentinel.prev = other._sentinel.prev;
                _sentinel.next->prev = &_sentinel;
                _sentinel.prev->next = &_sentinel;
                _size = other._size;

                other._sentinel.next = &other._sentinel;
                other._sentinel.prev = &other._sentinel;
                other._size = 0;
            }
            return *this;
        }

        ~List() override
        {
            clear();
        }

        // доступ

        reference front()
        {
            if (empty())
            {
                throw std::out_of_range("front() on empty list");
            }
            return _sentinel.next->value;
        }

        const_reference front() const
        {
            if (empty())
            {
                throw std::out_of_range("front() on empty list");
            }
            return _sentinel.next->value;
        }

        reference back()
        {
            if (empty())
            {
                throw std::out_of_range("back() on empty list");
            }
            return _sentinel.prev->value;
        }

        const_reference back() const
        {
            if (empty())
            {
                throw std::out_of_range("back() on empty list");
            }
            return _sentinel.prev->value;
        }

        // итераторы

        iterator begin() noexcept override
        {
            return iterator(_sentinel.next);
        }

        const_iterator begin() const noexcept override
        {
            return const_iterator(_sentinel.next);
        }

        const_iterator cbegin() const noexcept override
        {
            return const_iterator(_sentinel.next);
        }

        iterator end() noexcept override
        {
            return iterator(&_sentinel);
        }

        const_iterator end() const noexcept override
        {
            return const_iterator(&_sentinel);
        }

        const_iterator cend() const noexcept override
        {
            return const_iterator(&_sentinel);
        }

        // reverse итераторы

        reverse_iterator rbegin()
        {
            return reverse_iterator(end());
        }

        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator(end());
        }

        const_reverse_iterator crbegin() const
        {
            return const_reverse_iterator(end());
        }

        reverse_iterator rend()
        {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rend() const
        {
            return const_reverse_iterator(begin());
        }

        const_reverse_iterator crend() const
        {
            return const_reverse_iterator(begin());
        }

        // емкость

        bool empty() const noexcept override
        {
            return _size == 0;
        }

        size_type size() const noexcept override
        {
            return _size;
        }

        size_type max_size() const noexcept override
        {
            const size_type m = std::numeric_limits<size_type>::max() / sizeof(Node);
            if (m > 1)
            {
                return m - 1;
            }
            else
            {
                return 0;
            }
        }

        // модификаторы

        void clear() noexcept
        {
            Node *cur = _sentinel.next;
            while (cur != &_sentinel)
            {
                Node *next = cur->next;
                delete cur;
                cur = next;
            }
            _sentinel.next = &_sentinel;
            _sentinel.prev = &_sentinel;
            _size = 0;
        }

        iterator insert(const_iterator pos, const T &value)
        {
            return emplace_before(const_cast<Node *>(pos.raw()), value);
        }

        iterator insert(const_iterator pos, T &&value)
        {
            return emplace_before(const_cast<Node *>(pos.raw()), std::move(value));
        }

        iterator erase(const_iterator pos)
        {
            if (pos.raw() == &_sentinel)
            {
                throw std::out_of_range("erase(end()) is invalid");
            }
            Node *node = const_cast<Node *>(pos.raw());
            Node *next = node->next;
            unlink(node);
            delete node;
            --_size;
            return iterator(next);
        }

        void push_back(const T &value)
        {
            emplace_before(&_sentinel, value);
        }

        void push_back(T &&value)
        {
            emplace_before(&_sentinel, std::move(value));
        }

        void pop_back()
        {
            if (empty())
            {
                throw std::out_of_range("pop_back on empty list");
            }
            Node *node = _sentinel.prev;
            unlink(node);
            delete node;
            --_size;
        }

        void push_front(const T &value)
        {
            emplace_before(_sentinel.next, value);
        }

        void push_front(T &&value)
        {
            emplace_before(_sentinel.next, std::move(value));
        }

        void pop_front()
        {
            if (empty())
            {
                throw std::out_of_range("pop_front on empty list");
            }
            Node *node = _sentinel.next;
            unlink(node);
            delete node;
            --_size;
        }

        void resize(size_type count)
        {
            if (count < _size)
            {
                while (_size > count)
                {
                    pop_back();
                }
            }
            else if (count > _size)
            {
                while (_size < count)
                {
                    push_back(T{});
                }
            }
        }

        void swap(List &other) noexcept
        {
            if (this == &other)
            {
                return;
            }

            if (empty() && other.empty())
            {
            }
            else if (empty() && !other.empty())
            {
                _sentinel.next = other._sentinel.next;
                _sentinel.prev = other._sentinel.prev;
                _sentinel.next->prev = &_sentinel;
                _sentinel.prev->next = &_sentinel;

                other._sentinel.next = &other._sentinel;
                other._sentinel.prev = &other._sentinel;
            }
            else if (!empty() && other.empty())
            {
                other._sentinel.next = _sentinel.next;
                other._sentinel.prev = _sentinel.prev;
                other._sentinel.next->prev = &other._sentinel;
                other._sentinel.prev->next = &other._sentinel;

                _sentinel.next = &_sentinel;
                _sentinel.prev = &_sentinel;
            }
            else
            {
                std::swap(_sentinel.next->prev, other._sentinel.next->prev);
                std::swap(_sentinel.prev->next, other._sentinel.prev->next);
                std::swap(_sentinel.next, other._sentinel.next);
                std::swap(_sentinel.prev, other._sentinel.prev);
            }

            std::swap(_size, other._size);
        }

        // cравнение
        bool operator==(const Container<T, iterator, const_iterator> &rhs) const noexcept override
        {
            const auto &r = static_cast<const List &>(rhs);
            if (_size != r._size)
            {
                return false;
            }
            auto it1 = begin();
            auto it2 = r.begin();
            auto e1 = end();
            auto e2 = r.end();
            for (; it1 != e1 && it2 != e2; ++it1, ++it2)
            {
                if (!(*it1 == *it2))
                {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const Container<T, iterator, const_iterator> &rhs) const noexcept override
        {
            return !(*this == rhs);
        }

        bool operator<(const List &rhs) const
        {
            return std::lexicographical_compare(begin(), end(), rhs.begin(), rhs.end());
        }

        bool operator>(const List &rhs) const
        {
            return rhs < *this;
        }

        bool operator<=(const List &rhs) const
        {
            return !(*this > rhs);
        }

        bool operator>=(const List &rhs) const
        {
            return !(*this < rhs);
        }
    };

}
