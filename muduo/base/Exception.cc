#include "Exception.h"

#include "CurrentThread.h"

namespace muduo
{
    Exception::Exception(std::string msg) : msg_(std::move(msg)), stack_(CurrentThread::stack_trace(false))
    {
    }
}