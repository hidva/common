#ifndef ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_12_IO_ON_STREAM_H
#define ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_12_IO_ON_STREAM_H

#include <stdio.h>
#include "exception/errno_exception.h"
#include "glibc_cxx_wrap/macros.h"

inline FILE *cxx_fopen(const char *path, const char *mode)
{
    FILE *ptr = fopen(path,mode);
    if (ptr == nullptr)
        THROW(errno,"path: %s;mode: %s",path,mode);
    return ptr;
}

inline FILE *cxx_fdopen(int fd, const char *mode)
{
    FILE *ptr = nullptr;
    GLIBC_CXX_WRAP_NULL_ERRNO(ptr = fdopen(fd,mode),"fd: %d;mode: %s",fd,mode);
    return ptr;
}



#endif // ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_12_IO_ON_STREAM_H

