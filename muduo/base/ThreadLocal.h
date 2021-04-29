#pragma once

#include "Mutex.h"
#include "NonCopyable.h"

namespace muduo
{
    template<typename T>
    class ThreadLocal : NonCopyable
    {
    public:
        ThreadLocal()
        {
            MCHECK(pthread_key_create(&pkey_,&ThreadLocal::destructor));
        }

        ~ThreadLocal()
        {
            MCHECK(pthread_key_delete(pkey_));
        }

        T& value()
        {
            T* per_thread_value = static_cast<T*>(pthread_getspecific(pkey_));
            if(!per_thread_value)
            {
                T* new_obj = new T();
                MCHECK(pthread_setspecific(pkey_,new_obj));
                per_thread_value = new_obj;
            }
            return *per_thread_value;
        }

    private:
        static void destructor(void* x)
        {
            T* obj = static_cast<T*>(x);
            typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
            T_must_be_complete_type dummy;
            (void)dummy;
            delete obj;
        }

    private:
        pthread_key_t pkey_;
    };
}