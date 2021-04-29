#pragma once

#include <exception>
#include <string>

namespace muduo
{
    class Exception : public std::exception
    {
    public:
        explicit Exception(std::string msg);
        ~Exception() noexcept override = default;

        const char *what() const noexcept override
        {
            return msg_.c_str();
        }

        const char *stack() const noexcept
        {
            return stack_.c_str();
        }

    private:
        std::string msg_;
        std::string stack_;
    };
}