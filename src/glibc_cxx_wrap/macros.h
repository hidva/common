#ifndef ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_MACROS_H
#define ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_MACROS_H

#include "exception/errno_exception.h"

#define GLIBC_CXX_WRAP_ERRNO(fail_ret,statement,args...)             \
    do {                                                        \
        if ((statement) == (fail_ret))                               \
            THROW(errno,##args);                                \
    } while (0)

#define GLIBC_CXX_WRAP_1_ERRNO(statement,args...)               \
    GLIBC_CXX_WRAP_errno(-1,(statement),##args)

#define GLIBC_CXX_WRAP_NULL_ERRNO(statement,args...)            \
    GLIBC_CXX_WRAP_errno(nullptr,(statement),##args)

#define GLIBC_CXX_WARP_S0(statement,args...)                     \
    do {                                                        \
        int _GLIBC_CXX_WRAP_RET__ = (statement);                \
        if (_GLIBC_CXX_WRAP_RET__ != 0)                         \
            THROW(_GLIBC_CXX_WRAP_RET__ , ##args);              \
    } while (0)



#endif // ORG_PP_QQ_COMMON_GLIBC_CXX_WRAP_MACROS_H
