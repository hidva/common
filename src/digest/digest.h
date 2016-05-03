#ifndef ORG_PP_QQ_COMMON_DIGEST_DIGEST_H
#define ORG_PP_QQ_COMMON_DIGEST_DIGEST_H

#include <stddef.h>

struct IDigest {
    virtual void Update(const void *ptr,size_t s) = 0;
    virtual void Final(void *dst,size_t max_size) = 0;
    virtual void Reset() = 0;

    /* 这里加上 const 限定有意义么?还是暂且不要加了.
     */
    virtual int GetDigestSize() /* const */ = 0;
};


#endif // ORG_PP_QQ_COMMON_DIGEST_DIGEST_H
