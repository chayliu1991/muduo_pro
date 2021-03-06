#include "TimeStamp.h"

#include <sys/time.h>
#include <inttypes.h> //@ PRId64

namespace muduo
{
    static_assert(sizeof(TimeStamp) == sizeof(int64_t), "TimeStamp should be same size as int64_t");

    std::string TimeStamp::to_string() const
    {
        char buf[32] = {0};
        int64_t seconds = micro_seconds_since_epoch_ / kMicroSecondsPerSecond;
        int64_t micro_seconds = micro_seconds_since_epoch_ % kMicroSecondsPerSecond;
        snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64 "", seconds, micro_seconds);
        return buf;
    }

    std::string TimeStamp::to_formatted_string(bool show_micro_seconds) const
    {
        char buf[64] = {0};
        time_t seconds = static_cast<time_t>(micro_seconds_since_epoch_ / kMicroSecondsPerSecond);
        struct tm tm_time;
        gmtime_r(&seconds, &tm_time); //@ utc 时间

        if (show_micro_seconds)
        {
            int micro_seconds = static_cast<int>(micro_seconds_since_epoch_ % kMicroSecondsPerSecond);
            snprintf(buf, sizeof(buf), "%04d%02d%02d %02d:%02d:%02d.%06d", tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                     tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, micro_seconds);
        }
        else
        {
            snprintf(buf, sizeof(buf), "%04d%02d%02d %02d:%02d:%02d", tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                     tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        }
        return buf;
    }

    TimeStamp TimeStamp::now()
    {
        struct timeval tv;       //@ 精确到微秒
        gettimeofday(&tv, NULL); //@ 忽略时区
        int64_t seconds = tv.tv_sec;
        return TimeStamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
    }

}