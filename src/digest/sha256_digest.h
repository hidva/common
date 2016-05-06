#ifndef ORG_PP_QQ_COMMON_DIGEST_SHA256_DIGEST_H
#define ORG_PP_QQ_COMMON_DIGEST_SHA256_DIGEST_H

#include "nettle/sha.h"
#include "digest/digest.h"

struct SHA256Digest : public IDigest {
    enum {
        kDigestSize = 32
    };

    inline SHA256Digest() noexcept;

public:
    void Update(const void *ptr,size_t s) noexcept override;
    void Final(void *dst,size_t max_size) noexcept override;
    void Reset() noexcept override;
    int GetDigestSize() noexcept override;


private:
    sha256_ctx ctx_;
};


SHA256Digest::SHA256Digest() noexcept
{
    Reset();
}

#endif // ORG_PP_QQ_COMMON_DIGEST_SHA256_DIGEST_H
