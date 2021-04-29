#include "muduo/base/TimeStamp.h"
#include <vector>
#include <stdio.h>

using muduo::TimeStamp;

void passByConstReference(const TimeStamp &x)
{
    printf("%s\n", x.to_string().c_str());
    printf("%s\n", x.to_formatted_string().c_str());
}

void passByValue(TimeStamp x)
{
    printf("%s\n", x.to_string().c_str());
    printf("%s\n", x.to_formatted_string().c_str());
}

void benchmark()
{
    const int kNumber = 1000 * 1000;

    std::vector<TimeStamp> stamps;
    stamps.reserve(kNumber);
    for (int i = 0; i < kNumber; ++i)
    {
        stamps.push_back(TimeStamp::now());
    }
    printf("%s\n", stamps.front().to_string().c_str());
    printf("%s\n", stamps.back().to_string().c_str());
    printf("%f\n", time_difference(stamps.back(), stamps.front()));

    int increments[100] = {0};
    int64_t start = stamps.front().micro_seconds_since_epoch();
    for (int i = 1; i < kNumber; ++i)
    {
        int64_t next = stamps[i].micro_seconds_since_epoch();
        int64_t inc = next - start;
        start = next;
        if (inc < 0)
        {
            printf("reverse!\n");
        }
        else if (inc < 100)
        {
            ++increments[inc];
        }
        else
        {
            printf("big gap %d\n", static_cast<int>(inc));
        }
    }

    for (int i = 0; i < 100; ++i)
    {
        printf("%2d: %d\n", i, increments[i]);
    }
}

int main()
{
    TimeStamp now(TimeStamp::now());
    printf("%s\n", now.to_string().c_str());
    passByValue(now);
    passByConstReference(now);
    benchmark();
}
