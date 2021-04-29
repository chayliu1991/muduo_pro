#pragma once

#include <string>
#include <unistd.h>
#include <sys/syscall.h> //@ syscall
#include <sys/types.h>
#include <execinfo.h> //@ backtrace
#include <cxxabi.h>   //__abi::__cxa_demangle

namespace muduo
{
    namespace CurrentThread
    {
        extern __thread int t_cached_tid;
        extern __thread char t_tid_string[32];
        extern __thread int t_tid_string_length;
        extern __thread const char *t_thread_name;

        void cache_tid();

        inline int tid()
        {
            if (__builtin_expect(t_cached_tid == 0, 0))
            {
                cache_tid();
            }
            return t_cached_tid;
        }

        inline const char *tid_string()
        {
            return t_tid_string;
        }

        inline int tid_string_length()
        {
            return t_tid_string_length;
        }

        inline const char *name()
        {
            return t_thread_name;
        }

        pid_t gettid();

        bool is_main_thread();

        void sleep_usec(int64_t usec);

        std::string stack_trace(bool demangle);
    }
} // namespace name
