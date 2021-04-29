#include "Copyable.h"

#include <time.h>
#include <string>

namespace muduo
{
    class Date : public Copyable
    {
    public:
        struct YearMonthDay
        {
            int year; //@ [1900 2500]
            int month; //@ [1 12]
            int day; //@ [1 31]
        };

        static const int kDaysPerWeek = 7;
        static const int kJulianDayOf1970_01_01;

        Date():julian_day_number_(0){}
        explicit Date(int julianDayNum):julian_day_number_(julianDayNum){}
        Date(int year,int month,int day);
        explicit Date(const struct tm&);

        void swap(Date& that)
        {
            std::swap(julian_day_number_,that.julian_day_number_);
        }

        bool valid() const {return julian_day_number_ > 0;}
        //@ convert to yyyy-mm-dd format
        std::string to_iso_string() const;
        struct YearMonthDay year_month_day() const;

        int year()const
        {
            return year_month_day().year;
        }

        int month()const
        {
            return year_month_day().month;
        }

        //@ [0,1...6] => [Sunday,Monday...Saturday]
        int week_day() const
        {
            return (julian_day_number_ + 1) % kDaysPerWeek;
        }

        int julian_day_number() const
        {
            return julian_day_number_;
        }

    private:
        int julian_day_number_;

    };

    inline bool operator<(Date x,Date y)
    {
        return x.julian_day_number() < y.julian_day_number();
    }

    inline bool operator==(Date x,Date y)
    {
        return x.julian_day_number() == y.julian_day_number();
    }
}