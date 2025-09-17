#pragma once
#include <utility>
#include <cstddef>

namespace my_smart_ptr
{

    template <class T>
    class UniquePtr
    {
    public:
        constexpr UniquePtr() noexcept : ptr_(nullptr) {}
        explicit UniquePtr(T *ptr) noexcept : ptr_(ptr) {}

        UniquePtr(const UniquePtr &) = delete;
        UniquePtr &operator=(const UniquePtr &) = delete;

        UniquePtr(UniquePtr &&other) noexcept : ptr_(other.release()) {}
        UniquePtr &operator=(UniquePtr &&other) noexcept

        {
            if (this != &other)
            {
                reset(other.release());
            }
            return *this;
        }

        ~UniquePtr() noexcept
        {
            delete ptr_;
            ptr_ = nullptr;
        }

        T *get() const noexcept
        {
            return ptr_;
        }
        T &operator*() const
        {
            return *ptr_;
        }
        T *operator->() const noexcept
        {
            return ptr_;
        }
        explicit operator bool() const noexcept
        {
            return ptr_ != nullptr;
        }

        void reset(T *p = nullptr) noexcept
        {
            if (ptr_ != p)
            {
                delete ptr_;
                ptr_ = p;
            }
        }

        T *release() noexcept
        {
            return std::exchange(ptr_, nullptr);
        }

        void swap(UniquePtr &other) noexcept
        {
            std::swap(ptr_, other.ptr_);
        }

    private:
        T *ptr_;
    };

    template <class T>
    inline void swap(UniquePtr<T> &a, UniquePtr<T> &b) noexcept
    {
        a.swap(b);
    }

    template <class T>
    class UniquePtr<T[]>
    {
    public:
        constexpr UniquePtr() noexcept : ptr_(nullptr) {}
        explicit UniquePtr(T *ptr) noexcept : ptr_(ptr) {}

        UniquePtr(const UniquePtr &) = delete;
        UniquePtr &operator=(const UniquePtr &) = delete;

        explicit UniquePtr(UniquePtr &&other) noexcept : ptr_(other.release()) {}
        UniquePtr &operator=(UniquePtr &&other) noexcept
        {
            if (this != &other)
            {
                reset(other.release());
            }
            return *this;
        }

        ~UniquePtr() noexcept
        {
            delete[] ptr_;
            ptr_ = nullptr;
        }

        T *get() const noexcept { return ptr_; }
        explicit operator bool() const noexcept { return ptr_ != nullptr; }
        T &operator[](std::size_t i) const { return ptr_[i]; }

        void reset(T *p = nullptr) noexcept
        {
            (void)this;
            if (this->ptr_ != p)
            {
                delete[] this->ptr_;
                this->ptr_ = p;
            }
        }

        void swap(UniquePtr &other) noexcept
        {
            (void)this;
            std::swap(this->ptr_, other.ptr_);
        }

        T *release() noexcept
        {
            return std::exchange(ptr_, nullptr);
        }

    private:
        T *ptr_;
    };

    template <class T>
    inline void swap(UniquePtr<T[]> &a, UniquePtr<T[]> &b) noexcept
    {
        a.swap(b);
    }

}
