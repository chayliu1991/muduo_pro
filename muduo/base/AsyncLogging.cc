// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "AsyncLogging.h"
#include "LogFile.h"
#include "TimeStamp.h"

#include <stdio.h>

namespace muduo
{

    AsyncLogging::AsyncLogging(const std::string &basename,
                               off_t rollSize,
                               int flushInterval)
        : flush_interval_(flushInterval),
          running_(false),
          base_name_(basename),
          roll_size_(rollSize),
          thread_(std::bind(&AsyncLogging::thread_func, this), "Logging"),
          latch_(1),
          mutex_(),
          cond_(mutex_),
          current_buffer_(new Buffer),
          next_buffer_(new Buffer),
          buffers_()
    {
        current_buffer_->bzero();
        next_buffer_->bzero();
        buffers_.reserve(16);
    }

    void AsyncLogging::append(const char *logline, int len)
    {
        muduo::MutexLockGuard lock(mutex_);
        if (current_buffer_->avail() > len)
        {
            current_buffer_->append(logline, len);
        }
        else
        {
            buffers_.push_back(std::move(current_buffer_));

            if (next_buffer_)
            {
                current_buffer_ = std::move(next_buffer_);
            }
            else
            {
                current_buffer_.reset(new Buffer); // Rarely happens
            }
            current_buffer_->append(logline, len);
            cond_.notify();
        }
    }

    void AsyncLogging::thread_func()
    {
        assert(running_ == true);
        latch_.count_down();
        LogFile output(base_name_, roll_size_, false);
        BufferPtr newBuffer1(new Buffer);
        BufferPtr newBuffer2(new Buffer);
        newBuffer1->bzero();
        newBuffer2->bzero();
        BufferVector buffersToWrite;
        buffersToWrite.reserve(16);
        while (running_)
        {
            assert(newBuffer1 && newBuffer1->length() == 0);
            assert(newBuffer2 && newBuffer2->length() == 0);
            assert(buffersToWrite.empty());

            {
                muduo::MutexLockGuard lock(mutex_);
                if (buffers_.empty()) // unusual usage!
                {
                    cond_.wait_for_seconds(flush_interval_);
                }
                buffers_.push_back(std::move(current_buffer_));
                current_buffer_ = std::move(newBuffer1);
                buffersToWrite.swap(buffers_);
                if (!next_buffer_)
                {
                    next_buffer_ = std::move(newBuffer2);
                }
            }

            assert(!buffersToWrite.empty());

            if (buffersToWrite.size() > 25)
            {
                char buf[256];
                snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
                         TimeStamp::now().to_formatted_string().c_str(),
                         buffersToWrite.size() - 2);
                fputs(buf, stderr);
                output.append(buf, static_cast<int>(strlen(buf)));
                buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
            }

            for (const auto &buffer : buffersToWrite)
            {
                // FIXME: use unbuffered stdio FILE ? or use ::writev ?
                output.append(buffer->data(), buffer->length());
            }

            if (buffersToWrite.size() > 2)
            {
                // drop non-bzero-ed buffers, avoid trashing
                buffersToWrite.resize(2);
            }

            if (!newBuffer1)
            {
                assert(!buffersToWrite.empty());
                newBuffer1 = std::move(buffersToWrite.back());
                buffersToWrite.pop_back();
                newBuffer1->reset();
            }

            if (!newBuffer2)
            {
                assert(!buffersToWrite.empty());
                newBuffer2 = std::move(buffersToWrite.back());
                buffersToWrite.pop_back();
                newBuffer2->reset();
            }

            buffersToWrite.clear();
            output.flush();
        }
        output.flush();
    }
}