#include "muduo/base/ProcessInfo.h"
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

int main()
{
    printf("pid = %d\n", muduo::ProcessInfo::pid());
    printf("uid = %d\n", muduo::ProcessInfo::uid());
    printf("euid = %d\n", muduo::ProcessInfo::euid());
    printf("start time = %s\n", muduo::ProcessInfo::start_time().to_formatted_string().c_str());
    printf("hostname = %s\n", muduo::ProcessInfo::host_name().c_str());
    printf("opened files = %d\n", muduo::ProcessInfo::opened_files());
    printf("threads = %zd\n", muduo::ProcessInfo::threads().size());
    printf("num threads = %d\n", muduo::ProcessInfo::num_threads());
    printf("status = %s\n", muduo::ProcessInfo::proc_status().c_str());
}
