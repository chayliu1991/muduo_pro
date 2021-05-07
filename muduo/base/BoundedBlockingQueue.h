#pragma once

#include <list>
#include <assert.h>

#include "Condition.h"
#include "Mutex.h"

namespace muduo
{
    template <typename T>
    class BoundedBlockingQueue : NonCopyable
    {
    public:
        BoundedBlockingQueue(size_t max_size) : max_size_(max_size), mutex_(), not_empty_(mutex_), not_full_(mutex_) {}

        void put(const T &x)
        {
            MutexLockGuard lock(mutex_);
            while (is_full())
            {
                not_full_.wait();
            }
            assert(!is_full());
            data_.push_back(x);
            not_empty_.notify();
        }

        void put(T &&x)
        {
            MutexLockGuard lock(mutex_);
            while (is_full())
            {
                not_full_.wait();
            }
            assert(!is_full());
            data_.push_back(std::move(x));
            not_empty_.notify();
        }

        T take()
        {
            MutexLockGuard lock(mutex_);
            while (is_empty())
            {
                not_empty_.wait();
            }
            assert(!is_empty());
            T front(std::move(data_.front()));
            data_.pop_front();
            not_full_.notify();
            return front;
        }

        bool empty() const
        {
            MutexLockGuard lock(mutex_);
            return is_empty();
        }

        bool full() const
        {
            MutexLockGuard lock(mutex_);
            return is_full();
        }

        size_t size() const
        {
            MutexLockGuard lock(mutex_);
            return data_.size();
        }

        size_t capacity() const
        {
            MutexLockGuard lock(mutex_);
            return data_.capacity();
        }

    private:
        bool is_full() const
        {
            return data_.size() >= max_size_;
        }

        bool is_empty() const
        {
            return data_.empty();
        }

    private:
        size_t max_size_;
        mutable MutexLock mutex_;
        Condition not_empty_;
        Condition not_full_;
        std::list<T> data_;
    };
}