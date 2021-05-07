#pragma once

#include "BlockingQueue.h"
#include "BoundedBlockingQueue.h"
#include "CountDownLatch.h"
#include "Mutex.h"
#include "Thread.h"
#include "LogStream.h"

#include <atomic>
#include <vector>

namespace muduo
{
    class AsyncLogging : NonCopyable
    {
    public:
        AsyncLogging(const std::string &base_name, off_t roll_size, int flush_interval = 3);

        ~AsyncLogging()
        {
            if (running_)
            {
                stop();
            }
        }

        void append(const char *log_line, int len);

        void start()
        {
            running_ = true;
            thread_.start();
            latch_.wait();
        }

        void stop()
        {
            running_ = false;
            cond_.notify();
            thread_.join();
        }

    private:
        void thread_func();

        typedef muduo::detail::FixedBuffer<muduo::detail::kLargeBuffer> Buffer;
        typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
        typedef BufferVector::value_type BufferPtr;

        const int flush_interval_;
        std::atomic<bool> running_;
        const std::string base_name_;
        const off_t roll_size_;
        muduo::Thread thread_;
        muduo::CountDownLatch latch_;
        muduo::MutexLock mutex_;
        muduo::Condition cond_;
        BufferPtr current_buffer_;
        BufferPtr next_buffer_;
        BufferVector buffers_;
    };

}