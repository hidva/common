#ifndef ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_14_FILE_SYSTEM_INTERFACE_H
#define ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_14_FILE_SYSTEM_INTERFACE_H

#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include "exception/errno_exception.h"
#include "glibc_cxx_wrap/macros.h"
#include "rrid/on_exception.h"

inline DIR* cxx_opendir(const char *name)
{
    DIR *dirstream = ::opendir(name);
    if (dirstream == nullptr)
        THROW(errno,"path: %s",name);
    return dirstream;
}

inline DIR* cxx_fdopendir(int fd)
{
    DIR *dirstream = ::fdopendir(fd);
    if (dirstream == nullptr)
        THROW(errno,"fd: %d",fd);
    return dirstream;
}

inline int cxx_dirfd(DIR *dirp)
{
    int fd = ::dirfd(dirp);
    if (fd == -1)
        THROW(errno,"dirp: %p",dirp);
    return fd;
}

inline void cxx_chdir(const char *path)
{
    if (::chdir(path) != 0)
        THROW(errno,"path: %s",path);
    return ;
}

inline void cxx_fchdir(int fd)
{
    if (::fchdir(fd) != 0)
        THROW(errno,"fd: %d",fd);
    return ;
}


inline void cxx_readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{
    // The readdir_r() function returns 0 on success.  On error, it returns a positive error number.
    // 真是一点准备都没有.. 怎么不按照科学和其他 API 一样成功返回 0,出错返回 -1 呢!
    GLIBC_CXX_WARP_S0(readdir_r(dirp,entry,result),"dirp: %p;",dirp);
    return ;
}

inline bool Readdir(DIR *dirp, struct dirent *entry)
{
    struct dirent *end_flag = nullptr;
    cxx_readdir_r(dirp,entry,&end_flag);
    return end_flag == nullptr ? false : true;
}

inline char* cxx_getcwd (char *buffer, size_t size)
{
    if (::getcwd(buffer,size) == nullptr)
        THROW(errno,"buffer: %p,size: %zu",buffer,size);
    return buffer;
}

template <typename StringType>
void GetCWD(StringType &str)
{
    const size_t old_size = str.size();
    str.resize(old_size + PATH_MAX - 1);
    ON_EXCEPTIN {
        str.resize(old_size);
    };

    char *optr = const_cast<char*>(str.data() + old_size);
    cxx_getcwd(optr,PATH_MAX);
    str.resize(old_size + strlen(optr));
    return ;
}

template <typename StringType>
StringType GetCWD()
{
    StringType str;
    GetCWD(str);
    return str;
}


inline void cxx_stat(const char *pathname, struct stat *buf)
{
    GLIBC_CXX_WRAP_1_ERRNO(stat(pathname,buf),"path: %s,buf: %p",pathname,buf);
    return ;
}

inline void cxx_fstat(int fd, struct stat *buf)
{
    GLIBC_CXX_WRAP_1_ERRNO(fstat(fd,buf),"fd: %d,buf: %p",fd,buf);
    return ;
}

inline void cxx_lstat(const char *pathname, struct stat *buf)
{
    GLIBC_CXX_WRAP_1_ERRNO(lstat(pathname,buf),"path: %s,buf: %p",pathname,buf);
    return ;
}

inline void cxx_rename(const char *oldpath, const char *newpath)
{
    if (::rename(oldpath,newpath) != 0)
        THROW(errno,"oldpath: %s,newpath: %s",oldpath,newpath);
    return ;
}

inline char* cxx_mkdtemp(char *temp)
{
    char *ret = ::mkdtemp(temp);
    if (ret == nullptr)
        THROW(errno,"temp: %s",temp);
    return ret;
}

inline void cxx_unlink(const char *pathname)
{
    GLIBC_CXX_WRAP_1_ERRNO(unlink(pathname),"path: %s",pathname);
    return ;
}

inline void cxx_chmod(const char *pathname, mode_t mode)
{
    GLIBC_CXX_WRAP_1_ERRNO(chmod(pathname,mode),"path: %s;mode: %#x",pathname,mode);
    return ;
}

inline void cxx_fchmod(int fd, mode_t mode)
{
    GLIBC_CXX_WRAP_1_ERRNO(fchmod(fd,mode),"fd: %d;mode: %#x",fd,mode);
    return ;
}

inline mode_t cxx_umask (mode_t mask) noexcept
{
    // man 手册: This system call always succeeds and the previous value of the mask is returned.
    return umask(mask);
}

inline void cxx_mkdirat(int dirfd, const char *pathname, mode_t mode)
{
    GLIBC_CXX_WRAP_1_ERRNO(mkdirat(dirfd,pathname,mode),"dirfd: %d;path: %s;mode: %#x",dirfd,pathname,mode);
    return ;
}

inline void cxx_mkdir(const char *pathname, mode_t mode)
{
    GLIBC_CXX_WRAP_1_ERRNO(mkdir(pathname,mode),"path: %s;mode: %#x",pathname,mode);
    return ;
}

inline void cxx_rmdir(const char *pathname)
{
    GLIBC_CXX_WRAP_1_ERRNO(rmdir(pathname),"path: %s",pathname);
    return ;
}

inline char* cxx_realpath(const char *path, char *resolved_path)
{
    char *ret = nullptr;
    GLIBC_CXX_WRAP_NULL_ERRNO(ret = realpath(path,resolved_path),"path: %s",path);
    return ret;
}

/**
 * 将 realpath(path) 后的结果追加到 str 中.
 */
template <typename StringType>
void AppendRealpath(StringType &str,const char *path)
{
    const size_t old_size = str.size();

    str.resize(old_size + PATH_MAX - 1);
    ON_EXCEPTIN {
        str.resize(old_size);
    };

    char *optr = str.data() + old_size;
    cxx_realpath(path,optr);
    str.resize(old_size + strlen(optr));
    return ;
}

/**
 * 根据 path 的 realpath() 结果创建一个字符串对象.
 */
template <typename StringType>
inline StringType CreateStringFromRealPath(const char *path)
{
    StringType tmp;
    AppendRealpath(tmp,path);
    return tmp;
}



#endif // ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_14_FILE_SYSTEM_INTERFACE_H

