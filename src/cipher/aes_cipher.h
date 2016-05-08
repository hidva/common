
#ifndef ORG_PP_QQ_COMMON_CIPHER_AES_CIPHER_H_
#define ORG_PP_QQ_COMMON_CIPHER_AES_CIPHER_H_

#include "cipher/block_cipher.h"

#include "nettle/aes.h"

struct AESCipher : public BlockCipherImpl {
    void InitMode(int mode) override;
    void SetKey(const void *key,size_t size) noexcept override;

    void Encrypt(void *dst,const void *in) noexcept override;
    void Decrypt(void *dst,const void *in) noexcept override;

    unsigned GetBlockSize() const noexcept override;

public:

    enum : int{
        kBlockSize = 16
    };

private:
    struct aes_ctx ctx_;
    const uint8_t *key_ptr_ = nullptr;
    size_t key_size_ = 0;
};

#endif



