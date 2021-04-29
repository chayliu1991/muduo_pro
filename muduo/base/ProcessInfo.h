#pragma once

#include <vector>

#include "StringPiece.h"
#include "Types.h"
#include "TimeStamp.h"

namespace muduo
{
    namespace ProcessInfo
    {
        pid_t pid();
        std::string pid_string();
        uid_t uid();
        std::string user_name();
        uid_t euid();
        TimeStamp start_time();
        int clock_ticks_per_second();
        int page_size();
        bool is_debug_build();

        std::string host_name();
        std::string proc_name();
        StringPiece proc_name(const std::string &stat);

        std::string proc_status();
        std::string proc_stat();
        std::string thread_stat();
        std::string exe_path();

        int opened_files();
        int max_open_files();

        struct CpuTime
        {
            double user_seconds;
            double system_seconds;

            CpuTime() : user_seconds(0.0), system_seconds(0.0)
            {
            }

            double total() const { return user_seconds + system_seconds; }
        };

        CpuTime cpu_time();

        int num_threads();

        std::vector<pid_t> threads();
    }
}