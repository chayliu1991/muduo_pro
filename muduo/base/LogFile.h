#pragma once

#include "Mutex.h"
#include "Types.h"

#include <memory>

namespace muduo
{
    namespace FileUtil
    {
        class AppendFile;
    } //@ FileUtil

    class LogFile : NonCopyable
    {
    public:
        LogFile(const std::string &basename, off_t roll_size, bool thread_safe = true, int flush_interval = 3, int check_every_n = 1024);
        ~LogFile();

        void append(const char *log_line, int len);
        void flush();
        bool roll_file();

    private:
        void append_unlocked(const char *log_line, int len);

        static std::string get_log_file_name(const std::string &basename, time_t *now);

        const std::string base_name_;
        const off_t roll_size_;
        const int flush_interval_;
        const int check_every_n_;

        int count_;

        std::unique_ptr<MutexLock> mutex_;
        time_t start_of_period_;
        time_t last_roll_;
        time_t last_flush_;
        std::unique_ptr<FileUtil::AppendFile> file_;

        const static int kRollPerSeconds_ = 60 * 60 * 24;
    };
}