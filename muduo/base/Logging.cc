#include "Logging.h"
#include "CurrentThread.h"
#include "TimeZone.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

namespace muduo
{
    __thread char t_errno_buf[512];
    __thread char t_time[64];
    __thread time_t t_last_second;

    const char *strerror_tl(int savedErrno)
    {
        return strerror_r(savedErrno, t_errno_buf, sizeof(t_errno_buf));
    }

    Logger::LogLevel init_log_level()
    {
        if (::getenv("MUDUO_LOG_TRACE"))
            return Logger::TRACE;
        else if (::getenv("MUDUO_LOG_DEBUG"))
            return Logger::DEBUG;
        else
            return Logger::INFO;
    }

    Logger::LogLevel g_logLevel = init_log_level();

    const char *Log_level_name[Logger::NUM_LOG_LEVELS] =
        {
            "TRACE ",
            "DEBUG ",
            "INFO  ",
            "WARN  ",
            "ERROR ",
            "FATAL ",
    };

    // helper class for known string length at compile time
    class T
    {
    public:
        T(const char *str, unsigned len) : str_(str), len_(len)
        {
            assert(strlen(str) == len_);
        }

        const char *str_;
        const unsigned len_;
    };

    inline LogStream &operator<<(LogStream &s, T v)
    {
        s.append(v.str_, v.len_);
        return s;
    }

    inline LogStream &operator<<(LogStream &s, const Logger::SourceFile &v)
    {
        s.append(v.data_, v.size_);
        return s;
    }

    void default_output(const char *msg, int len)
    {
        size_t n = fwrite(msg, 1, len, stdout);
        //FIXME check n
        (void)n;
    }

    void default_flush()
    {
        fflush(stdout);
    }

    Logger::OutputFunc g_output = default_output;
    Logger::FlushFunc g_flush = default_flush;
    TimeZone g_logTimeZone;

    Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile &file, int line) : time_(TimeStamp::now()),
                                                                                           stream_(),
                                                                                           level_(level),
                                                                                           line_(line),
                                                                                           base_name_(file)
    {
        formatTime();
        CurrentThread::tid();
        stream_ << T(CurrentThread::tid_string(), CurrentThread::tid_string_length());
        stream_ << T(Log_level_name[level], 6);
        if (savedErrno != 0)
        {
            stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
        }
    }

    void Logger::Impl::formatTime()
    {
        int64_t microSecondsSinceEpoch = time_.micro_seconds_since_epoch();
        time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / TimeStamp::kMicroSecondsPerSecond);
        int microseconds = static_cast<int>(microSecondsSinceEpoch % TimeStamp::kMicroSecondsPerSecond);
        if (seconds != t_last_second)
        {
            t_last_second = seconds;
            struct tm tm_time;
            if (g_logTimeZone.valid())
            {
                tm_time = g_logTimeZone.to_local_time(seconds);
            }
            else
            {
                ::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime
            }

            int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
                               tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                               tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
            assert(len == 17);
            (void)len;
        }

        if (g_logTimeZone.valid())
        {
            Fmt us(".%06d ", microseconds);
            assert(us.length() == 8);
            stream_ << T(t_time, 17) << T(us.data(), 8);
        }
        else
        {
            Fmt us(".%06dZ ", microseconds);
            assert(us.length() == 9);
            stream_ << T(t_time, 17) << T(us.data(), 9);
        }
    }

    void Logger::Impl::finish()
    {
        stream_ << " - " << base_name_ << ':' << line_ << '\n';
    }

    Logger::Logger(SourceFile file, int line)
        : impl_(INFO, 0, file, line)
    {
    }

    Logger::Logger(SourceFile file, int line, LogLevel level, const char *func)
        : impl_(level, 0, file, line)
    {
        impl_.stream_ << func << ' ';
    }

    Logger::Logger(SourceFile file, int line, LogLevel level)
        : impl_(level, 0, file, line)
    {
    }

    Logger::Logger(SourceFile file, int line, bool toAbort)
        : impl_(toAbort ? FATAL : ERROR, errno, file, line)
    {
    }

    Logger::~Logger()
    {
        impl_.finish();
        const LogStream::Buffer &buf(stream().buffer());
        g_output(buf.data(), buf.length());
        if (impl_.level_ == FATAL)
        {
            g_flush();
            abort();
        }
    }

    void Logger::set_log_level(Logger::LogLevel level)
    {
        g_logLevel = level;
    }

    void Logger::set_output(OutputFunc out)
    {
        g_output = out;
    }

    void Logger::set_flush(FlushFunc flush)
    {
        g_flush = flush;
    }

    void Logger::set_time_zone(const TimeZone &tz)
    {
        g_logTimeZone = tz;
    }
}