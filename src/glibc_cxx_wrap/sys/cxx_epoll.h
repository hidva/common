#ifndef ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_EPOLL_H_
#define ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_EPOLL_H_

#include "exception/errno_exception.h"
#include "glibc_cxx_wrap/macros.h"
#include "common/config.h"

#include <errno.h>
#include <sys/epoll.h>
#include <linux/version.h>

#if defined(PP_QQ_COMMON_ON_ANDROID) && PP_QQ_COMMON_ON_ANDROID < 21

#include <unistd.h>
#include <sys/syscall.h>

enum {
    EPOLL_CLOEXEC = 02000000
#define EPOLL_CLOEXEC EPOLL_CLOEXEC
};

inline int epoll_create1 (int flags) noexcept
{
    return syscall(__NR_epoll_create1,flags);
}

#endif


inline int cxx_epoll_create(int size)
{
    int fd;
    GLIBC_CXX_WRAP_1_ERRNO(fd = epoll_create(size),"size: %d",size);
    return fd;
}

inline int cxx_epoll_create1(int flags)
{
    int fd ;
    GLIBC_CXX_WRAP_1_ERRNO(fd = epoll_create1(flags),"flags: %#x",flags);
    return fd;
}


inline void cxx_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
    GLIBC_CXX_WRAP_1_ERRNO(epoll_ctl(epfd,op,fd,event),"epfd: %d;op: %d;fd: %d",epfd,op,fd);
    return ;
}


inline int cxx_epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout)
{
    int ret;
    GLIBC_CXX_WRAP_1_ERRNO(ret = epoll_wait(epfd,events,maxevents,timeout),
        "epfd: %d;maxevents: %d,timeout: %d",epfd,maxevents,timeout);
    return ret;
}

#endif // ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_EPOLL_H_
