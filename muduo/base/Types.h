#pragma once

#include <string.h>

namespace muduo
{
    inline void mem_zero(void *p, size_t n)
    {
        memset(p, 0, n);
    }

    template <typename To, typename From>
    inline To implicit_cast(From const &f)
    {
        return f;
    }
}