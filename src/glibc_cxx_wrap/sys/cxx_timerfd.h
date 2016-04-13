#ifndef ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_TIMERFD_H_
#define ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_TIMERFD_H_

#include "exception/errno_exception.h"
#include "glibc_cxx_wrap/macros.h"

#include <sys/timerfd.h>

#if 0
#if defined(HAVE_SYS_TIMERFD_H_)
#include <sys/timerfd.h>
#else

#include <unistd.h>
#include <sys/syscall.h>

enum {
	TFD_CLOEXEC = 02000000,
#define TFD_CLOEXEC TFD_CLOEXEC
	TFD_NONBLOCK = 00004000
#define TFD_NONBLOCK TFD_NONBLOCK
};

enum {
	TFD_TIMER_ABSTIME = 1 << 0
#define TFD_TIMER_ABSTIME TFD_TIMER_ABSTIME
};

inline int timerfd_create (clockid_t clock_id, int flags) noexcept
{
	return syscall(__NR_timerfd_create,clock_id,flags);
}

inline int timerfd_settime (int ufd, int flags,const struct itimerspec *utmr,struct itimerspec *otmr) noexcept
{
	return syscall(__NR_timerfd_settime,ufd,flags,utmr,otmr);
}

inline int timerfd_gettime (int ufd, struct itimerspec *otmr) noexcept
{
	return syscall(__NR_timerfd_gettime,ufd,otmr);
}

#endif
#endif


inline int cxx_timerfd_create(int clockid, int flags)
{
	int fd;
    GLIBC_CXX_WRAP_1_ERRNO(fd = timerfd_create(clockid,flags),"clockid: %#x;flags: %#x",clockid,flags);
	return fd;
}

inline void cxx_timerfd_settime(int fd, int flags,const struct itimerspec *new_value,struct itimerspec *old_value)
{
    GLIBC_CXX_WRAP_1_ERRNO(timerfd_settime(fd,flags,new_value,old_value),"fd: %d;flags: %#x;",fd,flags);
	return ;
}

inline void cxx_timerfd_gettime(int fd, struct itimerspec *curr_value)
{
    GLIBC_CXX_WRAP_1_ERRNO(timerfd_gettime(fd,curr_value),"fd: %d",fd);
	return ;
}

#endif // ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_TIMERFD_H_
