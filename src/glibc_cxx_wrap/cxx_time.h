#ifndef ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_CXX_TIME_H
#define ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_CXX_TIME_H


#include <time.h>
#include "glibc_cxx_wrap/macros.h"

inline void cxx_clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    GLIBC_CXX_WRAP_1_ERRNO(clock_gettime(clk_id,tp),"clk_id: %d",clk_id);
    return ;
}

inline void cxx_clock_getres(clockid_t clk_id, struct timespec *res)
{
    GLIBC_CXX_WRAP_1_ERRNO(clock_getres(clk_id,res),"clk_id: %d",clk_id);
    return ;
}

inline void cxx_clock_settime(clockid_t clk_id, const struct timespec *tp)
{
    GLIBC_CXX_WRAP_1_ERRNO(clock_settime(clk_id,tp),"clk_id: %d;tp: {tv_sec: %lu;tv_nses: %lu}",clk_id,tp->tv_sec,tp->tv_nsec);
    return ;
}



#endif // ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_CXX_TIME_H
