#include "TimeZone.h"

#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <stdio.h>
#include <endian.h>
#include <assert.h>

#include "NonCopyable.h"
#include "Types.h"
#include "Date.h"

namespace muduo
{
    namespace detail
    {
        struct Transition
        {
            time_t gmt_time;
            time_t local_time;
            int local_time_index;

            Transition(time_t t, time_t l, int local_index) : gmt_time(t), local_time(l), local_time_index(local_index)
            {
            }
        };

        struct Comp
        {
            bool compare_gmt;

            Comp(bool gmt) : compare_gmt(gmt)
            {
            }

            bool operator()(const Transition &lhs, const Transition &rhs) const
            {
                if (compare_gmt)
                    return lhs.gmt_time < rhs.gmt_time;
                else
                    return lhs.local_time < rhs.local_time;
            }

            bool equal(const Transition &lhs, const Transition &rhs) const
            {
                if (compare_gmt)
                    return lhs.gmt_time == rhs.gmt_time;
                else
                    return lhs.local_time == rhs.local_time;
            }
        };

        struct LocalTime
        {
            time_t gmt_offset;
            bool is_dst;
            int arrb_index;

            LocalTime(time_t offset, bool dst, int arrb) : gmt_offset(offset), is_dst(dst), arrb_index(arrb)
            {
            }
        };

        inline void fill_hms(unsigned seconds, struct tm *utc)
        {
            utc->tm_sec = seconds % 60;
            unsigned minutes = seconds / 60;
            utc->tm_min = minutes;
            utc->tm_hour = minutes / 60;
        }
    }

    const int kSecondsPerDay = 24 * 60 * 60;

    struct TimeZone::Data
    {
        std::vector<detail::Transition> transitions;
        std::vector<detail::LocalTime> local_times;
        std::vector<std::string> names;
        std::string abbreviation;
    };

    namespace detail
    {
        class File : public NonCopyable
        {
        public:
            File(const char *file) : fp_(::fopen(file, "rb"))
            {
            }

            ~File()
            {
                if (fp_)
                {
                    ::fclose(fp_);
                }
            }

            bool valid() const { return fp_; }

            std::string read_bytes(int n)
            {
                char buf[n];
                ssize_t nr = ::fread(buf, 1, n, fp_);
                if (nr != n)
                    throw std::logic_error("no enough data");
                return std::string(buf, n);
            }

            int32_t read_int32()
            {
                int32_t x = 0;
                ssize_t nr = ::fread(&x, 1, sizeof(int32_t), fp_);
                if (nr != sizeof(int32_t))
                    throw std::logic_error("bad int32_t data");
                return be32toh(x); //@ convert byte order
            }

            uint8_t read_uint8()
            {
                uint8_t x = 0;
                ssize_t nr = ::fread(&x, 1, sizeof(uint8_t), fp_);
                if (nr != sizeof(uint8_t))
                    throw std::logic_error("bad uint8_t data");
                return x;
            }

        private:
            FILE *fp_;
        };

        bool read_time_zone_file(const char *zone_file, struct TimeZone::Data *data)
        {
            File f(zone_file);
            if (f.valid())
            {
                try
                {
                    std::string header = f.read_bytes(4);
                    if (header != "TZif")
                        throw std::logic_error("bad header");
                    std::string version = f.read_bytes(1);
                    f.read_bytes(15);

                    int32_t is_gmt_cnt = f.read_int32();
                    int32_t is_std_cnt = f.read_int32();
                    int32_t leap_cnt = f.read_int32();
                    int32_t time_cnt = f.read_int32();
                    int32_t type_cnt = f.read_int32();
                    int32_t char_cnt = f.read_int32();

                    std::vector<int32_t> trans;
                    std::vector<int> local_times;
                    trans.reserve(time_cnt);
                    for (int i = 0; i < time_cnt; ++i)
                    {
                        trans.push_back(f.read_int32());
                    }

                    for (int i = 0; i < time_cnt; ++i)
                    {
                        uint8_t local = f.read_uint8();
                        local_times.push_back(local);
                    }

                    for (int i = 0; i < type_cnt; ++i)
                    {
                        int32_t gmt_off = f.read_int32();
                        uint8_t is_dst = f.read_uint8();
                        uint8_t abbrind = f.read_uint8();
                        data->local_times.push_back(LocalTime(gmt_off, is_dst, abbrind));
                    }

                    for (int i = 0; i < time_cnt; ++i)
                    {
                        int local_idx = local_times[i];
                        time_t localtime = trans[i] + data->local_times[local_idx].gmt_offset;
                        data->transitions.push_back(Transition(trans[i], localtime, local_idx));
                    }

                    data->abbreviation = f.read_bytes(char_cnt);

                    for (int i = 0; i < leap_cnt; ++i)
                    {
                        // int32_t leaptime = f.read_int32();
                        // int32_t cumleap = f.read_int32();
                    }

                    (void)is_std_cnt;
                    (void)is_gmt_cnt;
                }
                catch (const std::logic_error &e)
                {
                    fprintf(stderr, "%s\n", e.what());
                }
            }
            return true;
        }

        const LocalTime *find_local_time(const TimeZone::Data &data, Transition sentry, Comp comp)
        {
            const LocalTime *local = NULL;

            if (data.transitions.empty() || comp(sentry, data.transitions.front()))
            {
                local = &data.local_times.front();
            }
            else
            {
                std::vector<Transition>::const_iterator trans_i = std::lower_bound(data.transitions.begin(), data.transitions.end(), sentry, comp);
                if (trans_i != data.transitions.end())
                {
                    if (!comp.equal(sentry, *trans_i))
                    {
                        assert(trans_i != data.transitions.begin());
                        --trans_i;
                    }

                    local = &data.local_times[trans_i->local_time_index];
                }
                else
                {
                    local = &data.local_times[data.transitions.back().local_time_index];
                }
            }
            return local;
        }

    } //@ namespace detail

    TimeZone::TimeZone(const char *zone_file) : data_(new TimeZone::Data)
    {
        if (!detail::read_time_zone_file(zone_file, data_.get()))
            data_.reset();
    }

    TimeZone::TimeZone(int east_of_utc, const char *tzname) : data_(new TimeZone::Data)
    {
        data_->local_times.push_back(detail::LocalTime(east_of_utc, false, 0));
        data_->abbreviation = tzname;
    }

    struct tm TimeZone::to_local_time(time_t seconds) const
    {
        struct tm local_time;
        mem_zero(&local_time, sizeof(local_time));
        assert(data_ != NULL);
        const Data &data(*data_);

        detail::Transition sentry(seconds, 0, 0);
        const detail::LocalTime *local = find_local_time(data, sentry, detail::Comp(true));
        if (local)
        {
            time_t local_seconds = seconds + local->gmt_offset;
            ::gmtime_r(&local_seconds, &local_time);
            local_time.tm_isdst = local->is_dst;
            local_time.tm_gmtoff = local->gmt_offset;
            local_time.tm_zone = &data.abbreviation[local->arrb_index];
        }
        return local_time;
    }

    time_t TimeZone::from_local_time(const struct tm &local) const
    {
        assert(data_ != NULL);
        const Data &data(*data_);
        struct tm tmp = local;
        time_t seconds = ::timegm(&tmp);
        detail::Transition sentry(0, seconds, 0);
        const detail::LocalTime *local_time = find_local_time(data, sentry, detail::Comp(false));
        if (local.tm_isdst)
        {
            struct tm try_tm = to_local_time(seconds - local_time->gmt_offset);
            if (!try_tm.tm_isdst && try_tm.tm_hour == local.tm_hour && try_tm.tm_min == local.tm_min)
            {
                seconds -= 3600;
            }
        }
        return seconds - local_time->gmt_offset;
    }

    struct tm TimeZone::to_utc_time(time_t seconds_since_epoch, bool yday)
    {
        struct tm utc;
        mem_zero(&utc, sizeof(utc));
        utc.tm_zone = "GMT";
        int seconds = static_cast<int>(seconds_since_epoch % kSecondsPerDay);
        int days = static_cast<int>(seconds_since_epoch / kSecondsPerDay);
        if (seconds < 0)
        {
            seconds += kSecondsPerDay;
            --days;
        }
        detail::fill_hms(seconds, &utc);
        Date date(days + Date::kJulianDayOf1970_01_01);
        Date::YearMonthDay ymd = date.year_month_day();
        utc.tm_year = ymd.year - 1900;
        utc.tm_mon = ymd.month - 1;
        utc.tm_mday = ymd.day;
        utc.tm_wday = date.week_day();

        if (yday)
        {
            Date start_of_year(ymd.year, 1, 1);
            utc.tm_yday = date.julian_day_number() - start_of_year.julian_day_number();
        }
        return utc;
    }

    time_t TimeZone::from_utc_time(const struct tm &utc)
    {
        return from_utc_time(utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday, utc.tm_hour, utc.tm_mon, utc.tm_sec);
    }

    time_t TimeZone::from_utc_time(int year, int month, int day, int hour, int minute, int seconds)
    {
        Date date(year, month, day);
        int seconds_in_day = hour * 3600 + minute * 60 + seconds;
        time_t days = date.julian_day_number() - Date::kJulianDayOf1970_01_01;
        return days * kSecondsPerDay + seconds_in_day;
    }
}