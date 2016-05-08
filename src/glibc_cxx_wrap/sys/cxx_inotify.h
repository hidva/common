#ifndef ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_INOTIFY_H_
#define ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_INOTIFY_H_

#include "exception/errno_exception.h"
#include "glibc_cxx_wrap/macros.h"
#include "common/config.h"

#include <sys/inotify.h>


#if defined(PP_QQ_COMMON_ON_ANDROID) && PP_QQ_COMMON_ON_ANDROID < 21
#include <unistd.h>
#include <sys/syscall.h>

enum {
    IN_CLOEXEC = 02000000,
#define IN_CLOEXEC IN_CLOEXEC
    IN_NONBLOCK = 00004000
#define IN_NONBLOCK IN_NONBLOCK
};

inline int inotify_init1 (int flags) noexcept
{
    return syscall(__NR_inotify_init1,flags);
}

#endif



inline int cxx_inotify_init()
{
    int fd;
    GLIBC_CXX_WRAP_1_ERRNO(fd = inotify_init());
    return fd;
}

inline int cxx_inotify_init1(int flags)
{
    int fd;
    GLIBC_CXX_WRAP_1_ERRNO(fd = inotify_init1(flags),"flags: %#x",flags);
    return fd;
}

inline int cxx_inotify_add_watch(int fd, const char *pathname, uint32_t mask)
{
    int wd;
    GLIBC_CXX_WRAP_1_ERRNO(wd = inotify_add_watch(fd,pathname,mask),"fd: %d;path: %s;mask: %#x",fd,pathname,mask);
    return wd;
}

inline void cxx_inotify_rm_watch(int fd, int wd)
{
    GLIBC_CXX_WRAP_1_ERRNO(inotify_rm_watch(fd,wd),"fd: %d;wd: %d",fd,wd);
    return ;
}

#endif // ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_INOTIFY_H_
