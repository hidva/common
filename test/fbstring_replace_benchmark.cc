
#include "folly/FBString.h"
#include "log/pp_qq_log.h"
#include "execute_time_get/execute_time_get.h"

using namespace folly;

int
main(int ,char **)
{
    const char buf[] {'1','2','3'};
    fbstring str {'h','e','l','w','o','r'};
    const char * const buf_begin = buf;
    const char * const buf_end = buf_begin + sizeof(buf) / sizeof(char);

    auto Replace = [&] () noexcept { str.replace(0,3,buf_begin,buf_end); };
    
    for (int i = 0; i < 7; ++i)
        PP_QQ_LOG_D("Replace time: %lu ns",ExecuteTimeGet(7,1000000,Replace));
    return 0;
}
