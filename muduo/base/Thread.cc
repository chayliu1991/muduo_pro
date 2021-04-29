#include "Thread.h"

#include <sys/prctl.h>

#include "Exception.h"

namespace muduo
{
    namespace detail
    {
        void after_fork()
        {
            CurrentThread::t_cached_tid = 0;
            CurrentThread::t_thread_name = "main";
            CurrentThread::tid();
        }

        struct ThreadNameInitializer
        {
            ThreadNameInitializer()
            {
                CurrentThread::t_thread_name = "main";
                CurrentThread::tid();
                pthread_atfork(NULL, NULL, &after_fork);
            }
        };

        ThreadNameInitializer init;

        struct ThreadData
        {
            typedef Thread::ThreadFunc ThrFunc;
            ThrFunc func;
            std::string name;
            pid_t *pid;
            CountDownLatch *latch;

            ThreadData(ThrFunc function, const std::string &thr_name, pid_t *process_id, CountDownLatch *count_down_latch) : func(function), name(thr_name), pid(process_id), latch(count_down_latch)
            {
            }

            void run_thread()
            {
                *pid = muduo::CurrentThread::tid();
                pid = NULL;
                latch->count_down();
                latch = NULL;

                CurrentThread::t_thread_name = name.empty() ? "unknown" : name.c_str();
                ::prctl(PR_SET_NAME, CurrentThread::t_thread_name);
                try
                {
                    func();
                    CurrentThread::t_thread_name = "finished";
                }
                catch (const Exception &ex)
                {
                    CurrentThread::t_thread_name = "crashed";
                    fprintf(stderr, "exception caught in Thread: %s\n", name.c_str());
                    fprintf(stderr, "reason: %s\n", ex.what());
                    fprintf(stderr, "stack trace: %s\n", ex.stack());
                    abort();
                }
                catch (const std::exception &ex)
                {
                    CurrentThread::t_thread_name = "crashed";
                    fprintf(stderr, "exception caught in Thread: %s\n", name.c_str());
                    fprintf(stderr, "reason: %s\n", ex.what());
                    abort();
                }
                catch (...)
                {
                    CurrentThread::t_thread_name = "crashed";
                    fprintf(stderr, "exception caught in Thread: %s\n", name.c_str());
                    throw; //@ rethrow
                }
            }
        };

        //@ global function
        void *start_thread(void *obj)
        {
            ThreadData *data = static_cast<ThreadData *>(obj);
            data->run_thread();
            delete data;
            return NULL;
        }
    } //@ namespace detail

    AtomicInt32 Thread::num_creadted_; //@ defination

    Thread::Thread(ThreadFunc func, const std::string &name) : started_(false), joined_(false), pthread_id_(0), pid_(0), func_(std::move(func)), name_(name), latch_(1)
    {
        set_default_name();
    }

    Thread::~Thread()
    {
        if (started_ && !joined_)
            pthread_detach(pthread_id_);
    }

    void Thread::set_default_name()
    {
        int num = num_creadted_.increment_and_get();
        if (name_.empty())
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "Thread%d", num);
            name_ = buf;
        }
    }

    void Thread::start()
    {
        assert(!started_);
        started_ = true;
        detail::ThreadData *data = new detail::ThreadData(func_, name_, &pid_, &latch_);
        if (pthread_create(&pthread_id_, NULL, &detail::start_thread, data))
        {
            started_ = false;
            delete data;
        }
        else
        {
            latch_.wait();
            assert(pid_ > 0);
        }
    }

    int Thread::join()
    {
        assert(started_);
        assert(!joined_);
        joined_ = true;
        return pthread_join(pthread_id_, NULL);
    }
}