#pragma once

#include "LogStream.h"
#include "TimeStamp.h"

namespace muduo
{
    class TimeZone;

    class Logger
    {
    public:
        enum LogLevel
        {
            TRACE,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            NUM_LOG_LEVELS,
        };

        // compile time calculation of basename of source file
        class SourceFile
        {
        public:
            template <int N>
            SourceFile(const char (&arr)[N]) : data_(arr), size_(N - 1)
            {
                const char *slash = strrchr(data_, '/'); // builtin function
                if (slash)
                {
                    data_ = slash + 1;
                    size_ -= static_cast<int>(data_ - arr);
                }
            }

            explicit SourceFile(const char *filename) : data_(filename)
            {
                const char *slash = strrchr(filename, '/');
                if (slash)
                {
                    data_ = slash + 1;
                }
                size_ = static_cast<int>(strlen(data_));
            }

            const char *data_;
            int size_;
        };

        Logger(SourceFile file, int line);
        Logger(SourceFile file, int line, LogLevel level);
        Logger(SourceFile file, int line, LogLevel level, const char *func);
        Logger(SourceFile file, int line, bool toAbort);
        ~Logger();

        LogStream &stream() { return impl_.stream_; }

        static LogLevel log_level();
        static void set_log_level(LogLevel level);

        typedef void (*OutputFunc)(const char *msg, int len);
        typedef void (*FlushFunc)();
        static void set_output(OutputFunc);
        static void set_flush(FlushFunc);
        static void set_time_zone(const TimeZone &tz);

    private:
        class Impl
        {
        public:
            typedef Logger::LogLevel LogLevel;
            Impl(LogLevel level, int old_errno, const SourceFile &file, int line);
            void formatTime();
            void finish();

            TimeStamp time_;
            LogStream stream_;
            LogLevel level_;
            int line_;
            SourceFile base_name_;
        };

        Impl impl_;
    };

    extern Logger::LogLevel g_logLevel;

    inline Logger::LogLevel Logger::log_level()
    {
        return g_logLevel;
    }

#define LOG_TRACE                                           \
    if (muduo::Logger::log_level() <= muduo::Logger::TRACE) \
    muduo::Logger(__FILE__, __LINE__, muduo::Logger::TRACE, __func__).stream()
#define LOG_DEBUG                                           \
    if (muduo::Logger::log_level() <= muduo::Logger::DEBUG) \
    muduo::Logger(__FILE__, __LINE__, muduo::Logger::DEBUG, __func__).stream()
#define LOG_INFO                                           \
    if (muduo::Logger::log_level() <= muduo::Logger::INFO) \
    muduo::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN muduo::Logger(__FILE__, __LINE__, muduo::Logger::WARN).stream()
#define LOG_ERROR muduo::Logger(__FILE__, __LINE__, muduo::Logger::ERROR).stream()
#define LOG_FATAL muduo::Logger(__FILE__, __LINE__, muduo::Logger::FATAL).stream()
#define LOG_SYSERR muduo::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL muduo::Logger(__FILE__, __LINE__, true).stream()

    const char *strerror_tl(int savedErrno);

#define CHECK_NOTNULL(val) \
    ::muduo::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

    // A small helper for CHECK_NOTNULL().
    template <typename T>
    T *CheckNotNull(Logger::SourceFile file, int line, const char *names, T *ptr)
    {
        if (ptr == NULL)
        {
            Logger(file, line, Logger::FATAL).stream() << names;
        }
        return ptr;
    }

}