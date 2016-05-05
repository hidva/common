#include <string.h>

#include "digest/sha256_digest.h"
#include "exception/errno_exception.h"


void SHA256Digest::Update(const void *ptr,size_t s) noexcept
{
    sha256_update(&ctx_,s,static_cast<const uint8_t*>(ptr));
    return ;
}

void SHA256Digest::Final(void *dst,size_t max_size) noexcept
{
    sha256_digest(&ctx_,max_size,static_cast<uint8_t*>(dst));
    return ;
}

void SHA256Digest::Reset() noexcept
{
    sha256_init(&ctx_);
    return ;
}

int SHA256Digest::GetDigestSize() noexcept
{
    return kDigestSize;
}
