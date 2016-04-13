
#include "log/pp_qq_log.h"
#include "execute_time_get/execute_time_get.h"

#include <thread>


void thread_main(unsigned char *addr,const char *thread_name)
{
    auto time_use = ExecuteTimeGet(7,300000000U,[&]{
        unsigned char var = *addr;
        ++var;
        *addr = var;
    });
    PP_QQ_LOG_D("thread: %s;time_use: %lu",thread_name,time_use);
}


#if 0
unsigned char data[2];
#else
unsigned char data[256];
#endif

int
main(int ,char **)
{
    std::thread thread1 {thread_main,&data[0],"thread1"};
    std::thread thread2 {thread_main,&data[sizeof(data) / sizeof(unsigned char) - 1],"thread2"};
    thread1.join();
    thread2.join();
    return 0;
}

