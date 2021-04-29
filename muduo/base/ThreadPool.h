#pragma once

#include <deque>
#include <vector>
#include <algorithm>
#include <memory>

#include "Thread.h"
#include "Mutex.h"
#include "Condition.h"
#include "NonCopyable.h"

namespace muduo
{
    class ThreadPool : public NonCopyable
    {
    public:
        typedef std::function<void(void)> Task;

        explicit ThreadPool(const std::string &name = std::string("ThreadPool"));
        ~ThreadPool();

        //@ call before start()
        void set_max_queue_size(int max_size) { max_queue_size_ = max_size; }
        void set_thread_init_callback(const Task &cb) { thread_init_callback_ = cb; }

        void start(int num_threads);
        void stop();

        const std::string &name() const
        {
            return name_;
        }

        size_t queue_size() const;

        void run(Task f);

    private:
        bool is_full() const;
        void run_in_thread();
        Task take();

    private:
        mutable MutexLock mutex_;
        Condition not_empty_;
        Condition not_full_;
        std::string name_;
        Task thread_init_callback_;
        std::vector<std::unique_ptr<Thread>> threads_;
        std::deque<Task> queue_;
        size_t max_queue_size_;
        bool running_;
    };
}