#include "CurrentThread.h"

namespace muduo
{
    namespace CurrentThread
    {
        __thread int t_cached_tid = 0;
        __thread char t_tid_string[32];
        __thread int t_tid_string_length = 6;
        __thread const char *t_thread_name = "unknown";

        static_assert(std::is_same<int, pid_t>::value, "pid should be int");

        std::string stack_trace(bool demangle)
        {
            std::string stack;
            const int kMaxFrames = 200;
            void *buf[kMaxFrames];
            int nptrs = ::backtrace(buf, kMaxFrames);
            char **strings = ::backtrace_symbols(buf, nptrs);
            if (strings)
            {
                size_t len = 256;
                char *demangled = demangle ? static_cast<char *>(::malloc(len)) : nullptr;
                for (int i = 1; i < nptrs; ++i) //@ i == 0,表示当前的函数，在此直接跳过
                {
                    if (demangle)
                    {
                        char *left_par = nullptr;
                        char *plus = nullptr;
                        for (char *p = strings[i]; *p; ++p)
                        {
                            if (*p == '(')
                                left_par = p;
                            else if (*p == '+')
                                plus = p;
                        }

                        if (left_par && plus)
                        {
                            *plus = '\0';
                            int status = 0;
                            char *ret = abi::__cxa_demangle(left_par + 1, demangled, &len, &status);
                            *plus = '+';
                            if (status == 0)
                            {
                                demangled = ret;
                                stack.append(strings[i], left_par + 1);
                                stack.append(demangled);
                                stack.append(plus);
                                stack.push_back('\n');
                                continue;
                            }
                        }
                    }
                    stack.append(strings[i]);
                    stack.push_back('\n');
                }
                free(demangled);
                free(strings);
            }
            return stack;
        }

        pid_t gettid()
        {
            return static_cast<pid_t>(::syscall(SYS_gettid));
        }

        void cache_tid()
        {
            if (t_cached_tid == 0)
            {
                t_cached_tid = gettid();
                t_tid_string_length = snprintf(t_tid_string, sizeof(t_tid_string), "%5d ", t_cached_tid);
            }
        }

        bool is_main_thread()
        {
            return tid() == getpid();
        }

        void sleep_usec(int64_t usec)
        {
            struct timespec ts = {0, 0}; //@ nano sleep
            ts.tv_sec = static_cast<time_t>(usec / (1000 * 1000));
            ts.tv_nsec = static_cast<long>(usec % (1000 * 1000 * 1000));
            ::nanosleep(&ts, NULL);
        }
    }

}