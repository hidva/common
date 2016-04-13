#ifndef ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_STAT_H_
#define ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_STAT_H_

#include "exception/errno_exception.h"
#include "glibc_cxx_wrap/macros.h"

#include <fcntl.h>
#include <sys/stat.h>


inline void cxx_utimensat(int dirfd, const char *pathname,const struct timespec times[2], int flags)
{
    GLIBC_CXX_WRAP_1_ERRNO(utimensat(dirfd,pathname,times,flags),"dirfd: %d;pathname: %s;flags: %#x",dirfd,pathname,flags);
    return ;
}

inline void cxx_futimens(int fd, const struct timespec times[2])
{
    GLIBC_CXX_WRAP_1_ERRNO(futimens(fd,times),"fd: %d",fd);
    return ;
}

#endif // ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_STAT_H_
