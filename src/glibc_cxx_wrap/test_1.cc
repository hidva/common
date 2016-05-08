#include "glibc_cxx_wrap/sys/cxx_inotify.h"
#include "glibc_cxx_wrap/sys/cxx_mman.h"
#include "glibc_cxx_wrap/sys/cxx_stat.h"
#include "glibc_cxx_wrap/sys/cxx_epoll.h"
#include "glibc_cxx_wrap/sys/cxx_timerfd.h"
#include "glibc_cxx_wrap/macros.h"
#include "glibc_cxx_wrap/cxx_time.h"
#include "glibc_cxx_wrap/13_low_level_io.h"
#include "glibc_cxx_wrap/14_file_system_interface.h"


/* test_1.cc test_2.cc 测试有 2 个目的:
 * 1. 测试是否有语法问题.
 * 2. 是否忘记追加 inline 属性;若某个函数忘记追加 inline 属性,则最后在链接时会提示符号重定义.
 */
