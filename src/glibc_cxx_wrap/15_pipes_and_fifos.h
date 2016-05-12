#ifndef ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_15_PIPES_AND_FIFOS_H
#define ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_15_PIPES_AND_FIFOS_H

#include "glibc_cxx_wrap/macros.h"
#include <stdio.h>

inline FILE* cxx_popen(const char *command, const char *type)
{
    FILE *stream = nullptr;
    GLIBC_CXX_WRAP_NULL_ERRNO(stream = popen(command,type),"cmd: %s,type: %s",command,type);
    return stream;
}

inline int cxx_pclose(FILE *stream)
{
#if 0
    // 首先可以确定的是 pclose() 失败时,一定会返回 -1,并且设置 errno 为错误码.
    // 但是当 pclose() 成功时,可能会返回 -1 么?不确定.
    int rc = -1;
    GLIBC_CXX_WRAP_1_ERRNO(rc = pclose(stream),"stream: %p",stream);
    return rc;
#endif

    errno = 0;
    int rc = pclose(stream);
    if (rc == -1 && errno != 0)
        THROW(errno,"stream: %p",stream);
    return rc;
}


#endif // ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_15_PIPES_AND_FIFOS_H

