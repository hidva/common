#ifndef ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_MMAN_H_
#define ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_MMAN_H_

#include "exception/errno_exception.h"
#include "glibc_cxx_wrap/macros.h"

#include <sys/mman.h>

inline void cxx_mprotect(void *addr, size_t len, int prot)
{
    GLIBC_CXX_WRAP_1_ERRNO(mprotect(addr,len,prot),"addr: %p;len: %zu;prot: %#x",addr,len,prot);
    return ;
}

#endif // ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_SYS_CXX_MMAN_H_
