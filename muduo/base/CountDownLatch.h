#pragma once
#include "Mutex.h"
#include "Condition.h"
#include "NonCopyable.h"

namespace muduo
{
    class CountDownLatch : public NonCopyable
    {
    public:
        explicit CountDownLatch(int count);

        void wait();
        void count_down();
        int get_count() const;

    private:
        mutable MutexLock mutex_;
        int count_;
        Condition condition_;
    };

} // namespace muduo
