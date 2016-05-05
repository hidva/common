
#ifndef ORG_PP_QQ_COMMON_CIPHER_AES_CIPHER_H_
#define ORG_PP_QQ_COMMON_CIPHER_AES_CIPHER_H_

#include "cipher/block_cipher.h"

#include <openssl/aes.h>

struct AESCipher : public BlockCipherImpl {
    void InitMode(int mode) override;
    void SetKey(const void *key,size_t size) override;

    void Encrypt(void *dst,const void *in) override;
    void Decrypt(void *dst,const void *in) override;

    unsigned GetBlockSize() const override;

public:

    enum : int{
        kBlockSize = 16
    };

private:
    AES_KEY ctx_;
    const unsigned char *key_ptr_ = nullptr;
    size_t key_size_bits_ = 0; // 单位是位,而不是字节!
};

#endif



