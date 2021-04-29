#pragma once
#include <pthread.h>
#include <assert.h>

#include "NonCopyable.h"
#include "CurrentThread.h"

namespace muduo
{

//@ 检查返回值的宏
#ifdef CHECK_PTHREAD_RETURN_VALUE

#ifdef NDEBUG
__BEGIN_DECLS
extern void __assert_perror_fail (int errnum,
                                  const char *file,
                                  unsigned int line,
                                  const char *function)
    noexcept __attribute__ ((__noreturn__));
__END_DECLS
#endif

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       if (__builtin_expect(errnum != 0, 0))    \
                         __assert_perror_fail (errnum, __FILE__, __LINE__, __func__);})

#else  // CHECK_PTHREAD_RETURN_VALUE

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       assert(errnum == 0); (void) errnum;})

#endif // CHECK_PTHREAD_RETURN_VALUE

    class MutexLock : public NonCopyable
    {
    public:
        MutexLock() : holder_(0)
        {
            MCHECK(pthread_mutex_init(&mutex_, nullptr));
        }

        ~MutexLock()
        {
            assert(holder_ == 0); //@ 保证锁不被持有
            MCHECK(pthread_mutex_destroy(&mutex_));
        }

        bool is_locked_by_this_thread() const
        {
            return holder_ == CurrentThread::tid();
        }

        void assert_locked() const
        {
            assert(is_locked_by_this_thread());
        }

        void lock()
        {
            MCHECK(pthread_mutex_lock(&mutex_));
            assign_holder();
        }

        void unlock()
        {
            unassign_holder();
            MCHECK(pthread_mutex_unlock(&mutex_));
        }

        pthread_mutex_t *get_mutex() //@ no const
        {
            return &mutex_;
        }

    private:
        void assign_holder()
        {
            holder_ = CurrentThread::tid();
        }

        void unassign_holder()
        {
            holder_ = 0;
        }

    private:
        friend class Condition; //@ Condition 需要使用到 UnassignGuard
        class UnassignGuard : NonCopyable
        {
        public:
            explicit UnassignGuard(MutexLock &owner) : owner_(owner)
            {
                owner_.unassign_holder();
            }

            ~UnassignGuard()
            {
                owner_.assign_holder();
            }

        private:
            MutexLock &owner_;  //@ 存放引用
        };

    private:
        pthread_mutex_t mutex_;
        pid_t holder_;
    };

    //@ RAII 格式 MutexLock 使用
    class MutexLockGuard : public NonCopyable
    {
    public:
        explicit MutexLockGuard(MutexLock &mutex) : mutex_(mutex)
        {
            mutex_.lock();
        }

        ~MutexLockGuard()
        {
            mutex_.unlock();
        }

    private:
        MutexLock &mutex_;
    };

//@ 避免构造一个临时的对象，例如 MutexLockGuard（mutex）,临时对象不能一直持有锁
#define MutexLockGuard(x) error "Missing guard object name"
}