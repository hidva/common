#include "digest/sha3_256.h"

void SHA3256Digest::Update(const void *ptr,size_t s) noexcept
{
    sha3_256_update(&ctx_,s,static_cast<const uint8_t*>(ptr));
    return ;
}

void SHA3256Digest::Final(void *dst,size_t max_size) noexcept
{
    sha3_256_digest(&ctx_,max_size,static_cast<uint8_t*>(dst));
    return ;
}

void SHA3256Digest::Reset() noexcept
{
    sha3_256_init(&ctx_);
    return ;
}

int SHA3256Digest::GetDigestSize() noexcept
{
    return kDigestSize;
}
