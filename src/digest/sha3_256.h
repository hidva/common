#ifndef ORG_PP_QQ_COMMON_DIGEST_SHA3_256_H
#define ORG_PP_QQ_COMMON_DIGEST_SHA3_256_H


#include "digest/digest.h"
#include "nettle/sha3.h"


struct SHA3256Digest : public IDigest {
    enum {
        kDigestSize = SHA3_256_DIGEST_SIZE
    };

    inline SHA3256Digest() noexcept;

public:
    void Update(const void *ptr,size_t s) noexcept override;
    void Final(void *dst,size_t max_size) noexcept override;
    void Reset() noexcept override;
    int GetDigestSize() noexcept override;


private:
    sha3_256_ctx ctx_;
};

SHA3256Digest::SHA3256Digest() noexcept
{
    Reset();
}


#endif // ORG_PP_QQ_COMMON_DIGEST_SHA3_256_H
