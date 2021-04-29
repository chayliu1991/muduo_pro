#pragma once
#include <pthread.h>
#include <assert.h>

#include "NonCopyable.h"

namespace muduo
{
    template <typename T>
    class ThreadLocalSingleton : public NonCopyable
    {
    public:
        ThreadLocalSingleton() = delete;
        ~ThreadLocalSingleton() = delete;

        static T& instance()
        {
            if(!t_value_)
            {
                t_value_ = new T();
                deleter.set(t_value_);
            }
            return *t_value_;
        }

        static T* pointer()
        {
            return t_value_;
        }

    private:
        static void destructor(vodi* obj)
        {
            assert(obj == t_value_);
            typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1: 1];
            T_must_be_complete_type dummy;
            (void)dummy;
            delete t_value_;
        }

        class Deleter
        {
            public:
            Deleter()
            {
                pthread_key_create(&pkey_,&ThreadLocalSingleton::destructor);
            }

            ~Deleter()
            {
                pthread_key_delete(&pkey_);
            }

            void set(T* new_obj)
            {
                assert(pthread_getspecific(pkey_) == NULL);
                pthread_setspecific(pkey_,new_obj);
            }

            pthread_key_t pkey_;
        }

    private:
        static __thread T* t_value_;
        static Deleter deleter;
    };

    template <typename T>
    __thread T* ThreadLocalSingleton<T>::t_value_ = 0;

    template <typename T>
    typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter;
}