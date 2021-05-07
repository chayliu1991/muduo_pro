#pragma once
#include <deque>
#include <assert.h>

#include "Condition.h"
#include "Mutex.h"

namespace muduo
{
    template <typename T>
    class BlockingQueue : NonCopyable
    {
    public:
        BlockingQueue() : mutex_(), not_empty_(mutex_) {}

        void put(const T &t)
        {
            MutexLockGuard lock(mutex_);
            data_.push_back(t);
            not_empty_.notify();
        }

        void put(T &&t)
        {
            MutexLockGuard lock(mutex_);
            data_.push_back(std::move(t));
            not_empty_.notify();
        }

        T take()
        {
            MutexLockGuard lock(mutex_);
            while (data_.empty())
            {
                not_empty_.wait();
            }
            assert(!data_.empty());
            T front(std::move(data_.front()));
            data_.pop_front();
            return front;
        }

        size_t size() const
        {
            MutexLockGuard lock(mutex_);
            return data_.size();
        }

    private:
        mutable MutexLock mutex_;
        Condition not_empty_;
        std::deque<T> data_;
    };
}