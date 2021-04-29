#include "ThreadPool.h"

#include "Exception.h"

namespace muduo
{
    ThreadPool::ThreadPool(const std::string &name) : mutex_(),
                                                      not_empty_(mutex_),
                                                      not_full_(mutex_),
                                                      name_(name),
                                                      max_queue_size_(0),
                                                      running_(false)
    {
    }

    ThreadPool::~ThreadPool()
    {
        if (running_)
        {
            stop();
        }
    }

    void ThreadPool::start(int num_threads)
    {
        assert(threads_.empty());
        running_ = true;
        threads_.reserve(num_threads);
        for (int i = 0; i < num_threads; ++i)
        {
            char id[32];
            snprintf(id, sizeof(id), "%d", i + 1);
            threads_.emplace_back(new Thread(std::bind(&ThreadPool::run_in_thread, this), name_ + id));
            threads_[i]->start();
        }

        if (num_threads == 0 && thread_init_callback_)
        {
            thread_init_callback_();
        }
    }

    void ThreadPool::stop()
    {
        {
            MutexLockGuard lock(mutex_);
            running_ = false;
            not_empty_.notify_all();
            not_full_.notify_all();
        }
        for (auto &thr : threads_)
        {
            thr->join();
        }
    }

    size_t ThreadPool::queue_size() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }

    void ThreadPool::run(Task task)
    {
        if (threads_.empty())
        {
            task();
        }
        else
        {
            MutexLockGuard lock(mutex_);
            while (is_full() && running_)
            {
                not_full_.wait();
            }
            if (!running_)
                return;
            assert(!is_full());

            queue_.push_back(std::move(task));
            not_empty_.notify();
        }
    }

    ThreadPool::Task ThreadPool::take()
    {
        MutexLockGuard lock(mutex_);
        while (queue_.empty() && running_)
        {
            not_empty_.wait();
        }

        Task task;
        if (!queue_.empty())
        {
            task = queue_.front();
            queue_.pop_front();
            if (max_queue_size_ > 0)
            {
                not_full_.notify();
            }
        }
        return task;
    }

    bool ThreadPool::is_full() const
    {
        mutex_.assert_locked();
        return max_queue_size_ > 0 && queue_.size() >= max_queue_size_;
    }

    void ThreadPool::run_in_thread()
    {
        try
        {
            if (thread_init_callback_)
            {
                thread_init_callback_();
            }
            while (running_)
            {
                Task task(take());
                if (task)
                {
                    task();
                }
            }
        }
        catch (const Exception &ex)
        {
            fprintf(stderr, "exception caught in ThreadPool %s \n", name_.c_str());
            fprintf(stderr, "exception reason: %s \n", ex.what());
            fprintf(stderr, "exception stack trace: %s \n", ex.stack());
            abort();
        }
        catch (const std::exception &ex)
        {
            fprintf(stderr, "exception caught in ThreadPool %s \n", name_.c_str());
            fprintf(stderr, "exception reason: %s \n", ex.what());
        }
        catch (...)
        {
            fprintf(stderr, "unknown exception caught in ThreadPool %s \n", name_.c_str());
            throw;
        }
    }
}
