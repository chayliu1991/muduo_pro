

#include "muduo/base/Atomic.h"

#include <assert.h>

int main()
{
    {
        muduo::AtomicInt64 a0;
        assert(a0.get() == 0);
        assert(a0.get_and_add(1) == 0);
        assert(a0.get() == 1);
        assert(a0.add_and_get(2) == 3);
        assert(a0.get() == 3);
        assert(a0.increment_and_get() == 4);
        assert(a0.get() == 4);
        a0.increment();
        assert(a0.get() == 5);
        assert(a0.add_and_get(-3) == 2);
        assert(a0.get_and_set(100) == 2);
        assert(a0.get() == 100);
    }

    {
        muduo::AtomicInt32 a1;
        assert(a1.get() == 0);
        assert(a1.get_and_add(1) == 0);
        assert(a1.get() == 1);
        assert(a1.add_and_get(2) == 3);
        assert(a1.get() == 3);
        assert(a1.increment_and_get() == 4);
        assert(a1.get() == 4);
        a1.increment();
        assert(a1.get() == 5);
        assert(a1.add_and_get(-3) == 2);
        assert(a1.get_and_set(100) == 2);
        assert(a1.get() == 100);
    }
}
