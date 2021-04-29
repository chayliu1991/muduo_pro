#include "muduo/base/FileUtil.h"

#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

using namespace muduo;

int main()
{
    std::string result;
    int64_t size = 0;
    int err = FileUtil::read_file("/proc/self", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::read_file("/proc/self", 1024, &result, NULL);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::read_file("/proc/self/cmdline", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::read_file("/dev/null", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::read_file("/dev/zero", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::read_file("/notexist", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::read_file("/dev/zero", 102400, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::read_file("/dev/zero", 102400, &result, NULL);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
}
