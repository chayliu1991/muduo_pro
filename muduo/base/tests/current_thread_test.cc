#include "muduo/base/CurrentThread.h"

#include <stdio.h>

using namespace muduo;

int main()
{
    if (CurrentThread::is_main_thread())
    {
        printf("this is main thread\n");
    }

    CurrentThread::sleep_usec(20);

    return 0;
}