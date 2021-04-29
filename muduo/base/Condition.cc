#include "Condition.h"

namespace muduo
{

    //@ returns true if time out, false otherwise.
    bool Condition::wait_for_seconds(double seconds)
    {
        struct timespec abs_time;
        clock_gettime(CLOCK_REALTIME,&abs_time);
        const int64_t kNanoSecondsPerSecond = 1000000000;
        int64_t nano_seconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);

        abs_time.tv_sec += static_cast<time_t>((abs_time.tv_sec + nano_seconds) / kNanoSecondsPerSecond);
        abs_time.tv_nsec += static_cast<long>((abs_time.tv_sec + nano_seconds) % kNanoSecondsPerSecond);

        MutexLock::UnassignGuard ug(mutex_);
        return ETIMEDOUT == pthread_cond_timedwait(&pcond_,mutex_.get_mutex(),&abs_time);
    }
}
