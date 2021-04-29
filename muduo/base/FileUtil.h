#pragma once

#include "NonCopyable.h"
#include "StringPiece.h"

namespace muduo
{
    namespace FileUtil
    {
        class ReadSmallFile : NonCopyable
        {
        public:
            ReadSmallFile(StringArg file_name);
            ~ReadSmallFile();

            //@ return errno
            template <typename String>
            int read_to_string(int max_size, String *content, int64_t *file_size, int64_t *modify_time, int64_t *create_time);

            //@ return errno
            int read_to_buffer(int *size);
            const char *buffer() const
            {
                return buf_;
            }

            const static int kBuffSize = 64 * 1024;

        private:
            int fd_;
            int err_;
            char buf_[kBuffSize];
        };

        template <typename String>
        int read_file(StringArg file_name, int max_size, String *content, int64_t *file_size = NULL, int64_t *modify_time = NULL, int64_t *create_time = NULL)
        {
            ReadSmallFile file(file_name);
            return file.read_to_string(max_size, content, file_size, modify_time, create_time);
        }

        //@ not thread safe
        class AppendFile : public NonCopyable
        {
        public:
            explicit AppendFile(StringArg file_name);
            ~AppendFile();

            void append(const char *log_line, size_t len);

            void flush();

            size_t write(const char *log_line, size_t len);

            off_t written_bytes() const
            {
                return written_bytes_;
            }

        private:
            FILE *fp_;
            char buffer_[64 * 1024];
            off_t written_bytes_;
        };
    }
}
