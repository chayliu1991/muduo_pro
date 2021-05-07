#include "LogFile.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "FileUtil.h"
#include "ProcessInfo.h"

namespace muduo
{
    LogFile::LogFile(const std::string &basename, off_t roll_size, bool thread_safe, int flush_interval, int check_every_n) : base_name_(basename),
                                                                                                                              roll_size_(roll_size),
                                                                                                                              flush_interval_(flush_interval),
                                                                                                                              check_every_n_(check_every_n),
                                                                                                                              count_(0),
                                                                                                                              mutex_(thread_safe ? new MutexLock : NULL),
                                                                                                                              start_of_period_(0),
                                                                                                                              last_roll_(0),
                                                                                                                              last_flush_(0)
    {
        assert(basename.find('/') == std::string::npos);
        roll_file();
    }

    LogFile::~LogFile() = default;

    void LogFile::append(const char *log_line, int len)
    {
        if (mutex_)
        {
            MutexLockGuard lock(*mutex_);
            append_unlocked(log_line, len);
        }
        else
        {
            append_unlocked(log_line, len);
        }
    }

    void LogFile::flush()
    {
        if (mutex_)
        {
            MutexLockGuard lock(*mutex_);
            file_->flush();
        }
        else
        {
            file_->flush();
        }
    }

    void LogFile::append_unlocked(const char *log_line, int len)
    {
        file_->append(log_line, len);

        if (file_->written_bytes() > roll_size_)
        {
            roll_file();
        }
        else
        {
            ++count_;
            if (count_ >= check_every_n_)
            {
                count_ = 0;
                time_t now = ::time(NULL);
                time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
                if (thisPeriod_ != start_of_period_)
                {
                    roll_file();
                }
                else if (now - last_flush_ > flush_interval_)
                {
                    last_flush_ = now;
                    file_->flush();
                }
            }
        }
    }

    bool LogFile::roll_file()
    {
        time_t now = 0;
        std::string filename = get_log_file_name(base_name_, &now);
        time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

        if (now > last_roll_)
        {
            last_roll_ = now;
            last_flush_ = now;
            start_of_period_ = start;
            file_.reset(new FileUtil::AppendFile(filename));
            return true;
        }
        return false;
    }

    std::string LogFile::get_log_file_name(const std::string &base_name, time_t *now)
    {
        std::string file_name;
        file_name.reserve(base_name.size() + 64);
        file_name = base_name;

        char timebuf[32];
        struct tm tm;
        *now = time(NULL);
        gmtime_r(now, &tm); // FIXME: localtime_r ?
        strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
        file_name += timebuf;
        file_name += ProcessInfo::host_name();

        char pidbuf[32];
        snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
        file_name += pidbuf;

        file_name += ".log";

        return file_name;
    }
}