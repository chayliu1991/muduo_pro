#include "muduo/base/ThreadPool.h"
#include "muduo/base/CountDownLatch.h"
#include "muduo/base/CurrentThread.h"

#include <stdio.h>
#include <unistd.h> // usleep

void print()
{
    printf("tid=%d\n", muduo::CurrentThread::tid());
}

void printString(const std::string &str)
{
    printf("printString :%s\n", str.c_str());
    usleep(100 * 1000);
}

void test(int maxSize)
{
    printf("Test ThreadPool with max queue size = %d\n", maxSize);
    muduo::ThreadPool pool("MainThreadPool");
    pool.set_max_queue_size(maxSize);
    pool.start(5);

    printf("Adding\n");
    pool.run(print);
    pool.run(print);
    for (int i = 0; i < 100; ++i)
    {
        char buf[32];
        snprintf(buf, sizeof buf, "task %d", i);
        pool.run(std::bind(printString, std::string(buf)));
    }
    printf("Done\n");

    muduo::CountDownLatch latch(1);
    pool.run(std::bind(&muduo::CountDownLatch::count_down, &latch));
    latch.wait();
    pool.stop();
}

/*
 * Wish we could do this in the future.
void testMove()
{
  muduo::ThreadPool pool;
  pool.start(2);

  std::unique_ptr<int> x(new int(42));
  pool.run([y = std::move(x)]{ printf("%d: %d\n", muduo::CurrentThread::tid(), *y); });
  pool.stop();
}
*/

void longTask(int num)
{
    printf("longTask %d begin\n", num);
    muduo::CurrentThread::sleep_usec(3000000);
    printf("longTask %d end\n", num);
}

void test2()
{
    printf("Test ThreadPool by stoping early.\n");
    muduo::ThreadPool pool("ThreadPool");
    pool.set_max_queue_size(5);
    pool.start(3);

    muduo::Thread thread1([&pool]() {
        for (int i = 0; i < 20; ++i)
        {
            pool.run(std::bind(longTask, i));
        }
    },
                          "thread1");

    thread1.start();

    muduo::CurrentThread::sleep_usec(5000000);
    printf("stop pool............................\n");
    pool.stop(); // early stop

    thread1.join();
    // run() after stop()
    pool.run(print);
    printf("test2 Done\n");
}

int main()
{
    test(0);
    test(1);
    test(5);
    test(10);
    test(50);
    test2();
}
