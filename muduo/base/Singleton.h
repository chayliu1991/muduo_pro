#pragma once

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include "NonCopyable.h"
#include <assert.h>

namespace muduo
{
    namespace detail
    {
        template <typename T>
        struct has_no_destroy
        {
            template <typename C>
            static char test(decltype(&C::no_destroy));

            template <typename C>
            static int32_t test(...);

            const static bool value = sizeof(test<T>(0)) == 1;
        };
    }

    template <typename T>
    class Singleton : public NonCopyable
    {
    public:
        Singleton() = delete;
        ~Singleton() = delete;

        static T &instance()
        {
            pthread_once(&once_, init);
            assert(value_);
            return *value_;
        }

    private:
        static void init()
        {
            value_ = new T();
            if (!detail::has_no_destroy<T>::value)
            {
                ::atexit(destroy);
            }
        }

        static void destroy()
        {
            typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
            T_must_be_complete_type dummy;
            (void)dummy;

            delete value_;
            value_ = NULL;
        }

    private:
        static pthread_once_t once_;
        static T *value_;
    };

    template <typename T>
    pthread_once_t Singleton<T>::once_ = PTHREAD_ONCE_INIT; //@ init it

    template <typename T>
    T *Singleton<T>::value_ = NULL;
}