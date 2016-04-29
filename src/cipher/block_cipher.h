#ifndef ORG_PP_QQ_COMMON_CIPHER_BLOCK_CIPHER_H
#define ORG_PP_QQ_COMMON_CIPHER_BLOCK_CIPHER_H


#include "cipher/icipher.h"

struct BlockCipherImpl {
    enum {
        kEncryptMode = 33,
        kDecryptMode = 77
    };
    virtual void InitMode(int mode) = 0;

    virtual void SetKey(const void *key,size_t size) = 0;

    virtual void Encrypt(void *dst,const void *in) = 0;
    virtual void Decrypt(void *dst,const void *in) = 0;

    virtual unsigned GetBlockSize() const = 0;
};

struct BlockCipher;

struct BlockCipherMode {
    virtual bool IsNeedPadding() const noexcept = 0;
    virtual void OnCipherBegin(BlockCipher *block_cipher) = 0;
    virtual void Update(BlockCipher *block_cipher,void *dst,const void *src,size_t size);
    virtual void OnCipherEnd(BlockCipher *block_cipher) = 0;
};

struct BlockCipherPaddingMode {

};

struct BlockCipher : public ICipher {
    inline BlockCipher(BlockCipherImpl *cipher_impl,BlockCipherMode *mode_impl,BlockCipherPaddingMode *pading_impl) noexcept;

public:
    CipherResult EncryptUpdate(void *dst,size_t max_size,const void *ptr,size_t size) override;
    CipherResult EncryptFinal(void *dst,size_t max_size,const void *ptr,size_t size) override;
    CipherResult EncryptUpdateOutputSize(size_t input_size) override;
    CipherResult EncryptFinalOutputSize(size_t input_size) override;

    CipherResult DecryptFinal(void *dst,size_t max_size,const void *ptr,size_t size) override;
    CipherResult DecryptUpdate(void *dst,size_t max_size,const void *ptr,size_t size) override;
    CipherResult DecryptUpdateOutputSize(size_t input_size) override;
    CipherResult DecryptFinalOutputSize(size_t input_size) override;

public:
    enum : int {
        kDontWork = 3,
        kEncrypting = 4,
        kDecrypting = 5
    };


    inline int GetCurrentStatus() const noexcept;
    inline BlockCipherImpl* GetCipherImpl() const noexcept;
    inline BlockCipherMode* GetCipherModeImpl() const noexcept;
    inline BlockCipherPaddingMode* GetPaddingModeImpl() const noexcept;

private:
    BlockCipherImpl *cipher_impl_ = nullptr;
    BlockCipherMode *mode_impl_ = nullptr;
    BlockCipherPaddingMode *padding_impl_ = nullptr;

    int status_ = kDontWork;
};

BlockCipher::BlockCipher(BlockCipherImpl *cipher_impl,BlockCipherMode *mode_impl,BlockCipherPaddingMode *pading_impl) noexcept:
    cipher_impl_(cipher_impl),
    mode_impl_(mode_impl),
    padding_impl_(pading_impl)
{
}

int BlockCipher::GetCurrentStatus() const noexcept
{
    return status_;
}

BlockCipherImpl* BlockCipher::GetCipherImpl() const noexcept
{
    return cipher_impl_;
}

BlockCipherMode* BlockCipher::GetCipherModeImpl() const noexcept
{
    return mode_impl_;
}

BlockCipherPaddingMode* BlockCipher::GetPaddingModeImpl() const noexcept
{
    return padding_impl_;
}

#endif // ORG_PP_QQ_COMMON_CIPHER_BLOCK_CIPHER_H
