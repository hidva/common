
#ifndef ORG_PP_QQ_COMMON_EXECUTE_TIME_GET_EXECUTE_TIME_GET_H_
#define ORG_PP_QQ_COMMON_EXECUTE_TIME_GET_EXECUTE_TIME_GET_H_

#include "common/inline_utils.h"
#include "glibc_cxx_wrap/cxx_time.h"
#include <stdint.h>

#if !defined(UINT_FAST64_MAX)
#define UINT_FAST64_MAX 0xFFFFFFFFFFFFFFFFULL
#endif


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

        uint_fast64_t current_time_use = GetTimespecDiff(&end_time,&begin_time);
        if (current_time_use < time_use_min)
            time_use_min = current_time_use;
    }

    return time_use_min;
}



#endif /* ORG_PP_QQ_COMMON_EXECUTE_TIME_GET_EXECUTE_TIME_GET_H_ */
