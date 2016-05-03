#ifndef ORG_PP_QQ_COMMON_DIGEST_SHA256_DIGEST_H
#define ORG_PP_QQ_COMMON_DIGEST_SHA256_DIGEST_H

#include <openssl/sha.h>
#include "digest/digest.h"

struct SHA256Digest : public IDigest {
    enum {
        kDigestSize = 32
    };
public:
    void Update(const void *ptr,size_t s) override;
    void Final(void *dst,size_t max_size) override;
    void Reset() override;
    int GetDigestSize() override;


private:
    SHA256_CTX ctx_;
};




#endif // ORG_PP_QQ_COMMON_DIGEST_SHA256_DIGEST_H
