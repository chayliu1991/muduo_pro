#pragma once
#include <pthread.h>
#include <functional>
#include <memory>
#include <string>

#include "NonCopyable.h"
#include "CountDownLatch.h"
#include "Atomic.h"
#include "CurrentThread.h"

namespace muduo
{
    class Thread : public NonCopyable
    {
    public:
        typedef std::function<void()> ThreadFunc;
        explicit Thread(ThreadFunc func, const std::string &name = std::string());
        ~Thread();

        void start();
        int join();

        bool started() const { return started_; }
        pid_t tid() const { return pid_; }
        const std::string &name() { return name_; }

        static int num_created() { return num_creadted_.get(); }

    private:
        void set_default_name();

    private:
        bool started_;
        bool joined_;
        pthread_t pthread_id_;
        pid_t pid_;
        ThreadFunc func_;
        std::string name_;
        CountDownLatch latch_;

        static AtomicInt32 num_creadted_;
    };
}