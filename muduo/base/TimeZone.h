#pragma once
#include <time.h>
#include <memory>

#include "Copyable.h"

namespace muduo
{
    class TimeZone : public Copyable
    {
    public:
        TimeZone() = default;
        explicit TimeZone(const char *zone_file);
        TimeZone(int east_of_utc, const char *tzname);

        bool valid() const
        {
            return static_cast<bool>(data_);
        }

        struct tm to_local_time(time_t seconds_since_epoch) const;
        time_t from_local_time(const struct tm &) const;

        static struct tm to_utc_time(time_t seconds_since_epoch, bool yday = false);
        static time_t from_utc_time(const struct tm &);
        static time_t from_utc_time(int year, int month, int day, int hour, int minute, int seconds);

        struct Data;

    private:
        std::shared_ptr<Data> data_;
    };
}