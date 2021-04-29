#pragma once

#include "Mutex.h"
#include  <pthread.h>

namespace muduo
{
    class Condition
    {
    public:
        Condition(MutexLock& mutex) : mutex_(mutex)
        {
            MCHECK(pthread_cond_init(&pcond_,NULL));
        }

        ~Condition()
        {
            MCHECK(pthread_cond_destroy(&pcond_));
        }

        void wait()
        {
            //@ pthread_cond_wait will release mutex
            MutexLock::UnassignGuard ug(mutex_);
            MCHECK(pthread_cond_wait(&pcond_, mutex_.get_mutex()));
        }

        //@ returns true if time out, false otherwise.
        bool wait_for_seconds(double seconds);

        void notify()
        {
            MCHECK(pthread_cond_signal(&pcond_));
        }

        void notify_all()
        {
            MCHECK(pthread_cond_broadcast(&pcond_));
        }

    private:
        MutexLock& mutex_;
        pthread_cond_t pcond_;
    };
}