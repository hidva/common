#include <string.h>

#include "digest/sha256_digest.h"
#include "exception/errno_exception.h"


void SHA256Digest::Update(const void *ptr,size_t s)
{
    PP_QQ_CHECK(SHA256_Update(&ctx_,ptr,s) == 1);
    return ;
}

void SHA256Digest::Final(void *dst,size_t max_size)
{
    if (max_size >= kDigestSize) {
        PP_QQ_CHECK(SHA256_Final(static_cast<unsigned char*>(dst),&ctx_) == 1);
    } else {
        unsigned char buffer[kDigestSize];
        PP_QQ_CHECK(SHA256_Final(buffer,&ctx_) == 1);
        memcpy(dst,buffer,max_size);
    }

    Reset();
    return ;
}

void SHA256Digest::Reset()
{
    PP_QQ_CHECK(SHA256_Init(&ctx_) == 1);
    return ;
}

int SHA256Digest::GetDigestSize()
{
    return kDigestSize;
}
