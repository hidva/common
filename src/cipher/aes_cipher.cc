
#include <errno.h>
#include "exception/errno_exception.h"
#include "cipher/aes_cipher.h"

namespace {
using AES_set_key_func_p = void (*) (struct aes_ctx *ctx, size_t length,const uint8_t *key);
constexpr AES_set_key_func_p g_AES_set_key_func[] {aes_set_encrypt_key,aes_set_decrypt_key};
}

void AESCipher::InitMode(int mode)
{
    if (key_size_ != AES128_KEY_SIZE && key_size_ != AES192_KEY_SIZE && key_size_ != AES256_KEY_SIZE)
        THROW(EINVAL,"key_size: %zu",key_size_);

    g_AES_set_key_func[mode == kDecryptMode](&ctx_,key_size_,key_ptr_);
    return ;
}

void AESCipher::SetKey(const void *key,size_t size) noexcept
{
    key_ptr_ = static_cast<const uint8_t*>(key);
    key_size_ = size;
    return ;
}

void AESCipher::Encrypt(void *dst,const void *in) noexcept
{
    aes_encrypt(&ctx_,kBlockSize,static_cast<uint8_t*>(dst),static_cast<const uint8_t*>(in));
    return ;
}

void AESCipher::Decrypt(void *dst,const void *in) noexcept
{
    aes_decrypt(&ctx_,kBlockSize,static_cast<uint8_t*>(dst),static_cast<const uint8_t*>(in));
    return ;
}

unsigned AESCipher::GetBlockSize() const noexcept
{
    return kBlockSize;
}


