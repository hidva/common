
#include "glibc_cxx_wrap/13_low_level_io.h"

size_t ReadN(int fd,void *buf,size_t count)
{
    // 该处代码已经经过测试.
    unsigned char *optr = static_cast<char*>(buf);
    size_t osize = count;
    while (osize > 0) {
        ssize_t rc = read(fd,optr,osize);
        if (rc == -1) {
            if (errno != EINTR)
                THROW(errno,"fd: %d,buf: %p,count: %zu",fd,buf,count);
        } else if (rc == 0) {
            break;
        } else {
            optr += rc;
            osize -= rc;
        }
    }
    return count - osize;
}

size_t PreadN(int fd, void *buf, size_t count, off_t offset)
{
    unsigned char *optr = static_cast<unsigned char*>(buf);
    size_t osize = count;

    while (osize > 0) {
        ssize_t rc = pread(fd,optr,osize,offset);
        if (rc == -1) {
            if (errno != EINTR)
                THROW(errno,"fd: %d,buf: %p,count: %zu,offset: %lld",fd,buf,count,static_cast<long long>(offset));
        } else if (rc == 0) {
            break;
        } else {
            offset += rc;
            optr += rc;
            osize -= rc;
        }
    }

    return count - osize;
}

void WriteN(int fd,const void *buf,size_t count)
{
    const unsigned char *iptr = static_cast<const unsigned char*>(buf);
    size_t isize = count;

    while (isize > 0) {
        auto rc = write(fd,iptr,isize);
        if (rc == -1) {
            if (errno != EINTR)
                THROW(errno,"fd: %d,buf: %p,count: %zu,iptr: %p,isize: %zu",fd,buf,count,iptr,isize);
        } else {
            iptr += rc;
            isize -= rc;
        }
    }

    return ;
}

void PwriteN(int fd,const void *buf,size_t count,off_t offset)
{
    const unsigned char *iptr = static_cast<const unsigned char*>(buf);
    size_t isize = count;

    while (isize > 0) {
        auto rc = pwrite(fd,iptr,isize,offset);
        if (rc == -1) {
            if (errno != EINTR)
                THROW(errno,"fd: %d,buf: %p,count: %zu,iptr: %p,isize: %zu",fd,buf,count,iptr,isize);
        } else {
            iptr += rc;
            isize -= rc;
            offset += rc;
        }
    }

    return ;
}

