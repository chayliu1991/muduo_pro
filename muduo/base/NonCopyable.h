#pragma once

namespace muduo
{
    class NonCopyable
    {
    public:
        NonCopyable(const NonCopyable &) = delete;
        void operator=(const NonCopyable &) = delete;

    protected:
        NonCopyable() = default;
        ~NonCopyable() = default;
    };
}