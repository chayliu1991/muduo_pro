#include "CountDownLatch.h"

namespace muduo
{
    CountDownLatch::CountDownLatch(int count) : mutex_(), count_(count), condition_(mutex_)
    {
    }

    void CountDownLatch::wait()
    {
        MutexLockGuard lock(mutex_);
        while (count_ > 0)
        {
            condition_.wait();
        }
    }

    void CountDownLatch::count_down()
    {
        MutexLockGuard lock(mutex_);
        --count_;
        if (count_ == 0)
        {
            condition_.notify_all();
        }
    }

    int CountDownLatch::get_count() const
    {
        MutexLockGuard lock(mutex_);
        return count_;
    }
}