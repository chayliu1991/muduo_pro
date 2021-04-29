#pragma once
#include <cstdint>
#include <string>

#include "Copyable.h"

namespace muduo
{
    //@ 源代码还继承自  boost::equality_comparable<Timestamp>, boost::less_than_comparable<Timestamp>
    class TimeStamp : public Copyable
    {
    public:
        static const int kMicroSecondsPerSecond = 1000 * 1000;

        TimeStamp() : micro_seconds_since_epoch_(0) {}
        explicit TimeStamp(int64_t micro_seconds) : micro_seconds_since_epoch_(micro_seconds) {}

        void swap(TimeStamp &that)
        {
            std::swap(micro_seconds_since_epoch_, that.micro_seconds_since_epoch_);
        }

        std::string to_string() const;
        std::string to_formatted_string(bool show_micro_seconds = true) const;

        bool valid() const { return micro_seconds_since_epoch_ > 0; }
        int64_t micro_seconds_since_epoch() const { return micro_seconds_since_epoch_; }
        int64_t secnods_since_epoch() const
        {
            return static_cast<time_t>(micro_seconds_since_epoch_ / kMicroSecondsPerSecond);
        }

        static TimeStamp now();
        static TimeStamp invalid()
        {
            return TimeStamp(); //@ 获取一个无效的时间戳
        }

        static TimeStamp from_unix_time(time_t t)
        {
            return from_unix_time(t, 0);
        }

        static TimeStamp from_unix_time(time_t t, int micro_seconds)
        {
            return TimeStamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + micro_seconds);
        }

    private:
        int64_t micro_seconds_since_epoch_;
    };

    inline bool operator<(TimeStamp lhs, TimeStamp rhs)
    {
        return lhs.micro_seconds_since_epoch() < rhs.micro_seconds_since_epoch();
    }

    inline bool operator==(TimeStamp lhs, TimeStamp rhs)
    {
        return lhs.micro_seconds_since_epoch() == rhs.micro_seconds_since_epoch();
    }

    inline double time_difference(TimeStamp high, TimeStamp low)
    {
        int64_t diff = high.secnods_since_epoch() - low.secnods_since_epoch();
        return static_cast<double>(diff) / TimeStamp::kMicroSecondsPerSecond;
    }

    inline TimeStamp add_time(TimeStamp time_stamp, double seconds)
    {
        int64_t delta = static_cast<int64_t>(seconds * TimeStamp::kMicroSecondsPerSecond);
        return TimeStamp(time_stamp.secnods_since_epoch() + delta);
    }
}