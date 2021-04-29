#pragma once

#include <stdint.h>

#include "NonCopyable.h"

namespace muduo
{
    namespace detail
    {
        template <typename T>
        class AtomicInteger : NonCopyable
        {
        public:
            AtomicInteger() : val_(0) {}

            T get()
            {
                // in gcc >= 4.7: __atomic_load_n(&value_, __ATOMIC_SEQ_CST)
                return __sync_val_compare_and_swap(&val_, 0, 0);
            }

            T get_and_add(T x)
            {
                // in gcc >= 4.7: __atomic_load_n(&value_, __ATOMIC_SEQ_CST)
                return __sync_fetch_and_add(&val_, x);
            }

            T add_and_get(T x)
            {
                return get_and_add(x) + x;
            }

            T increment_and_get()
            {
                return add_and_get(1);
            }

            T decrement_and_get()
            {
                return add_and_get(-1);
            }

            void add(T x)
            {
                get_and_add(x);
            }

            void increment()
            {
                increment_and_get();
            }

            void decrement()
            {
                decrement_and_get();
            }

            T get_and_set(T newValue)
            {
                // in gcc >= 4.7: __atomic_exchange_n(&value_, newValue, __ATOMIC_SEQ_CST)
                return __sync_lock_test_and_set(&val_, newValue);
            }

        private:
            T val_;
        };
    }

    //@ 预定义两个常用的类型
    typedef detail::AtomicInteger<int32_t> AtomicInt32;
    typedef detail::AtomicInteger<int64_t> AtomicInt64;
} // namespace muduo
