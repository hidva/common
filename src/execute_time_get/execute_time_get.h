
#ifndef ORG_PP_QQ_COMMON_EXECUTE_TIME_GET_EXECUTE_TIME_GET_H_
#define ORG_PP_QQ_COMMON_EXECUTE_TIME_GET_EXECUTE_TIME_GET_H_

#include "glibc_cxx_wrap/cxx_time.h"
#include <stdint.h>

namespace execute_time_get {

namespace detail {


/* 计算 left - right;并将结果转化为纳秒.
 * @param left,right 调用方确保 left,right normalized!!!并且 left >= right!!!
 * @warning 当 left,right 之间相差太大时,可能会溢出.比如相差 560 年... T_T
 */
inline uint_fast64_t GetTimespecDiff(const struct timespec *left,const struct timespec *right) noexcept
{
    uint_fast64_t la = left->tv_sec;
    uint_fast64_t lb = left->tv_nsec;
    uint_fast64_t ra = right->tv_sec;
    uint_fast64_t rb = right->tv_nsec;
    return (la - ra) * 1000000000 + (lb - rb);
}


}

}

template <typename F>
uint_fast64_t ExecuteTimeGet(size_t iter,size_t times,F &&func)
{
    uint_fast64_t time_use_min = UINT_FAST64_MAX;

    for (size_t i = 0; i < iter; ++i) {
        struct timespec begin_time,end_time;
        cxx_clock_gettime(CLOCK_REALTIME,&begin_time);
        for (size_t j = 0; j < times; ++j)
            func();
        cxx_clock_gettime(CLOCK_REALTIME,&end_time);

        uint_fast64_t current_time_use = ::execute_time_get::detail::GetTimespecDiff(&end_time,&begin_time);
        if (current_time_use < time_use_min)
            time_use_min = current_time_use;
    }

    return time_use_min;
}



#endif /* ORG_PP_QQ_COMMON_EXECUTE_TIME_GET_EXECUTE_TIME_GET_H_ */
