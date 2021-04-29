#include "FileUtil.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Types.h"

namespace muduo
{
    FileUtil::ReadSmallFile::ReadSmallFile(StringArg file_name) : fd_(::open(file_name.c_str(), O_RDONLY | O_CLOEXEC)), err_(0)
    {
        buf_[0] = '\0';
        if (fd_ < 0)
        {
            err_ = errno;
        }
    }

    FileUtil::ReadSmallFile::~ReadSmallFile()
    {
        if (fd_ >= 0)
        {
            ::close(fd_);
        }
    }

    template <typename String>
    int FileUtil::ReadSmallFile::read_to_string(int max_size, String *content, int64_t *file_size, int64_t *modify_time, int64_t *create_time)
    {
        static_assert(sizeof(off_t) == 8, "_FILE_OFFSET_BITS = 64");
        assert(concept != NULL);
        int err = err_;
        if (fd_ >= 0)
        {
            content->clear();
            if (file_size)
            {
                struct stat stat_buf;
                if (::fstat(fd_, &stat_buf) == 0)
                {
                    if (S_ISREG(stat_buf.st_mode))
                    {
                        *file_size = stat_buf.st_size;
                        content->reserve(static_cast<int>(std::min(implicit_cast<int64_t>(max_size), *file_size)));
                    }
                    else if (S_ISDIR(stat_buf.st_mode))
                    {
                        err = EISDIR;
                    }
                    if (modify_time)
                    {
                        *modify_time = static_cast<int64_t>(stat_buf.st_mtim.tv_sec);
                    }
                    if (create_time)
                    {
                        *create_time = static_cast<int64_t>(stat_buf.st_ctim.tv_sec);
                    }
                }
                else
                {
                    err = errno;
                }
            }

            while (content->size() < implicit_cast<size_t>(max_size))
            {
                size_t to_read = std::min(implicit_cast<size_t>(max_size) - content->size(), sizeof(buf_));
                ssize_t n = ::read(fd_, buf_, to_read);
                if (n > 0)
                {
                    content->append(buf_, n);
                }
                else
                {
                    if (n < 0)
                    {
                        err = errno;
                    }
                    break;
                }
            }
        }
        return err;
    }

    int FileUtil::ReadSmallFile::read_to_buffer(int *size)
    {
        int err = err_;
        if (fd_ > 0)
        {
            ssize_t n = ::pread(fd_, buf_, sizeof(buf_) - 1, 0);
            if (n >= 0)
            {
                if (size)
                {
                    *size = static_cast<int>(n);
                }
                buf_[n] = '\0';
            }
        }
        else
        {
            err = errno;
        }
        return err;
    }

    //@ specialization
    template int FileUtil::read_file(StringArg filename, int maxSize, std::string *content, int64_t *, int64_t *, int64_t *);
    template int FileUtil::ReadSmallFile::read_to_string(int maxSize, std::string *content, int64_t *, int64_t *, int64_t *);

    //@ 'e' for O_CLOEXEC
    FileUtil::AppendFile::AppendFile(StringArg file_name) : fp_(::fopen(file_name.c_str(), "ae")), written_bytes_(0)
    {
        assert(fp_);
        ::setbuffer(fp_, buffer_, sizeof(buffer_));
    }

    FileUtil::AppendFile::~AppendFile()
    {
        ::fclose(fp_);
    }

    void FileUtil::AppendFile::append(const char *log_line, size_t len)
    {
        size_t n = write(log_line, len);
        size_t remain = len - n;
        while (remain > 0)
        {
            size_t x = write(log_line + n, remain);
            if (x == 0)
            {
                int err = ferror(fp_);
                if (err)
                {
                    // fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
                }
                break;
            }
            n += x;
            remain = len - n;
        }
        written_bytes_ += len;
    }

    void FileUtil::AppendFile::flush()
    {
        ::fflush(fp_);
    }

    size_t FileUtil::AppendFile::write(const char *log_line, size_t len)
    {
        return ::fwrite_unlocked(log_line, 1, len, fp_);
    }
}