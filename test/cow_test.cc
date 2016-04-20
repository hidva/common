
#include "execute_time_get/execute_time_get.h"
#include "log/pp_qq_log.h"

#include <string>
#include <folly/FBString.h>


int
main(int ,char **)
{
    {
        std::string str (10 * 1024 * 1024,'c');
        std::string str1 = str;
        PP_QQ_LOG_D("std::string: %s",str1.data() == str.data() ? "采用 COW" : "没采用 COW");

        folly::fbstring str2 (10 * 1024 * 1024,'c');
        folly::fbstring str3 = str2;
        PP_QQ_LOG_D("folly::fbstring: %s",str2.data() == str3.data() ? "采用 COW" : "没采用 COW");
    }
    
    {
        std::string str (10 * 1024 * 1024,'c');
        std::string str1;
        
        PP_QQ_LOG_D("Time: %lu ns",ExecuteTimeGet(7,1000,[&] () noexcept {str1 = str;}));
    }    

    {
        folly::fbstring str (10 * 1024 * 1024,'c');
        folly::fbstring str1;
        
        PP_QQ_LOG_D("Time: %lu ns",ExecuteTimeGet(7,1000,[&] () noexcept {str1 = str;}));
    }        
    return 0;
}
