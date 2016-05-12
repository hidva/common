#ifndef ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_13_LOW_LEVEL_IO_H
#define ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_13_LOW_LEVEL_IO_H


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdarg.h>

#include "glibc_cxx_wrap/macros.h"
#include "rrid/scope_exit.h"

#if !defined(O_TMPFILE)
#define O_TMPFILE 020200000
#endif

inline bool OpenHaveModeArg(int oflag) noexcept
{
    return (((oflag) & O_CREAT) != 0 || ((oflag) & O_TMPFILE) == O_TMPFILE);
}

inline int vopen(const char *filename,int flags,va_list ap)
{
    return OpenHaveModeArg(flags) ? open(filename,flags,va_arg(ap,int)) : open(filename,flags);
}

inline int cxx_vopen(const char *filename,int flags,va_list ap)
{
    int ret;
    GLIBC_CXX_WRAP_1_ERRNO(ret = vopen(filename,flags,ap),"filename: %s;flags: %#x",filename,flags);
    return ret;
}

inline int cxx_open(const char *path,int flags,...)
{
    va_list ap;
    va_start(ap,flags);
    ON_SCOPE_EXIT (va_end_ap) {
        va_end(ap);
    };

    return cxx_vopen(path,flags,ap);
}

inline int vopenat(int dirfd, const char *pathname, int flags, va_list ap)
{
    return OpenHaveModeArg(flags) ? openat(dirfd,pathname,flags,va_arg(ap,int)) : openat(dirfd,pathname,flags);
}

inline int cxx_vopenat(int dirfd, const char *pathname, int flags, va_list ap)
{
    int fd = -1;
    GLIBC_CXX_WRAP_1_ERRNO(fd = vopenat(dirfd,pathname,flags,ap),"dirfd: %d,pathname: %s,flags: %#x",dirfd,pathname,flags);
    return fd;
}

inline int cxx_openat(int dirfd, const char *pathname, int flags, ...)
{
    va_list ap;
    va_start(ap,flags);
    ON_SCOPE_EXIT (va_end_ap) {
        va_end(ap);
    };

    return cxx_vopenat(dirfd,pathname,flags,ap);
}


inline int vfcntl(int fd,int cmd,va_list ap)
{
    return fcntl(fd,cmd,va_arg(ap,void*));
}

inline int cxx_vfcntl(int fd,int cmd,va_list ap)
{
    int ret;
    GLIBC_CXX_WRAP_1_ERRNO(ret = vfcntl(fd,cmd,ap),"fd: %d;cmd: %d",fd,cmd);
    return ret;
}

inline int cxx_fcntl(int fd,int cmd,...)
{
    va_list ap;
    va_start(ap,cmd);
    ON_SCOPE_EXIT (va_end_ap) {
        va_end(ap);
    };

    return cxx_vfcntl(fd,cmd,ap);
}

inline bool IsNonblocking(int fd)
{
    return (cxx_fcntl(fd,F_GETFL) & O_NONBLOCK);
}

inline void SetNonblocking(int fd)
{
    cxx_fcntl(fd, F_SETFL, cxx_fcntl(fd, F_GETFL) | O_NONBLOCK);
    return ;
}

inline void SetBlocking(int fd)
{
    cxx_fcntl(fd, F_SETFL, cxx_fcntl(fd, F_GETFL) & ~O_NONBLOCK);
    return ;
}

inline size_t cxx_read(int fd, void *buf, size_t count)
{
    ssize_t ret = 0;
    GLIBC_CXX_WRAP_1_ERRNO(ret = read(fd,buf,count),"fd: %d;buf: %p;count: %zu",fd,buf,count);
    return ret;
}

inline size_t cxx_write(int fd, const void *buf, size_t count)
{
    ssize_t ret = 0;
    GLIBC_CXX_WRAP_1_ERRNO(ret = write(fd,buf,count),"fd: %d;buf: %p;count: %zu",fd,buf,count);
    return ret;
}


inline void* cxx_mmap(void *address,size_t length,int protect,int flags,int filedes,off_t offset)
{
    void *ret = nullptr;
    GLIBC_CXX_WRAP_ERRNO(MAP_FAILED,
        ret = mmap(address,length,protect,flags,filedes,offset),
        "address: %p;length: %zu;prot: %#x;flags: %#x;fd: %d;offset: %ld",
        address,length,protect,flags,filedes,offset);
    return ret;
}

inline void cxx_munmap(void *addr,size_t length)
{
    GLIBC_CXX_WRAP_1_ERRNO(munmap(addr,length),"addr: %p;size: %zu",addr,length);
    return ;
}

inline void cxx_msync (void *address, size_t length, int flags)
{
    GLIBC_CXX_WRAP_1_ERRNO(msync(address,length,flags),
        "addr: %p;length: %zu;flags: %d",
        address,length,flags);
    return ;
}

inline size_t cxx_readv(int fd, const struct iovec *iov, int iovcnt)
{
    int ret = -1;
    GLIBC_CXX_WRAP_1_ERRNO(ret = readv(fd,iov,iovcnt),"fd: %d,iov: %p,iovcnt: %d",fd,iov,iovcnt);
    return ret;
}

inline size_t cxx_writev(int fd, const struct iovec *iov, int iovcnt)
{
    int ret = writev(fd,iov,iovcnt);
    if (ret == -1)
        THROW(errno,"fd: %d,iov: %p,iovcnt: %d",fd,iov,iovcnt);
    return ret;
}

inline off_t cxx_lseek (int filedes, off_t offset, int whence)
{
    off_t ret = lseek(filedes,offset,whence);
    if (ret == -1)
        THROW(errno,"fd: %d;offset: %ld;whence: %d",filedes,offset,whence);
    return ret;
}

inline size_t cxx_pwrite64(int fd, const void * buf, size_t count, off64_t offset)
{
    ssize_t ret = pwrite64(fd,buf,count,offset);
    if (ret == -1)
        THROW(errno,"fd: %d;buf: %p;count: %zu;offset:%lld",fd,buf,count,static_cast<long long>(offset));
    return ret;
}

inline size_t cxx_pread64(int fd, void * buf, size_t count, off64_t offset)
{
    ssize_t ret = pread64(fd,buf,count,offset);
    if (ret == -1)
        THROW(errno,"fd: %d;buf: %p;count: %zu;offset:%lld",fd,buf,count,static_cast<long long>(offset));
    return ret;
}

inline size_t cxx_pwrite(int fd, const void * buf, size_t count, off_t offset)
{
    ssize_t ret = pwrite(fd,buf,count,offset);
    if (ret == -1)
        THROW(errno,"fd: %d;buf: %p;count: %zu;offset:%ld",fd,buf,count,offset);
    return ret;
}

inline size_t cxx_pread(int fd, void * buf, size_t count, off_t offset)
{
    ssize_t ret = pread(fd,buf,count,offset);
    if (ret == -1)
        THROW(errno,"fd: %d;buf: %p;count: %zu;offset:%ld",fd,buf,count,offset);
    return ret;
}

// 获取 fd 当前偏移.
inline size_t GetCurrentOffset(int fd)
{
    return cxx_lseek(fd,0,SEEK_CUR);
}

inline int vioctl(int d, unsigned long request, va_list ap)
{
    return ioctl(d,request,va_arg(ap,void*));
}

inline int cxx_vioctl(int d,unsigned long request,va_list ap)
{
    int ret;
    GLIBC_CXX_WRAP_1_ERRNO(ret = vioctl(d,request,ap));
    return ret;
}

inline int cxx_ioctl(int d,unsigned long request,...)
{
    va_list ap;
    va_start(ap,request);
    ON_SCOPE_EXIT (va_end_ap) {
        va_end(ap);
    };

    return cxx_vioctl(d,request,ap);
}

inline int cxx_dup(int oldfd)
{
    int ret;
    GLIBC_CXX_WRAP_1_ERRNO(ret = dup(oldfd),"oldfd: %d",oldfd);
    return ret;
}

inline int cxx_dup2(int oldfd, int newfd)
{
    int ret;
    GLIBC_CXX_WRAP_1_ERRNO(ret = dup2(oldfd,newfd),"oldfd: %d,newfd: %d",oldfd,newfd);
    return ret;
}

inline void cxx_close(int fd)
{
    GLIBC_CXX_WRAP_1_ERRNO(close(fd),"fd: %d",fd);
    return ;
}

#endif // ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_13_LOW_LEVEL_IO_H

