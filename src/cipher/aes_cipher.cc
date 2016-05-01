
#include <errno.h>
#include "exception/errno_exception.h"
#include "cipher/aes_cipher.h"

constexpr auto g_AES_set_key_func[] {AES_set_encrypt_key,AES_set_decrypt_key};

void AESCipher::InitMode(int mode)
{
    int rc = (g_AES_set_key_func[mode == kDecryptMode])(key_ptr_,key_size_bits_,&ctx_);
    if (rc != 0)
        THROW(EINVAL,"mode: %d;rc: %d",mode,rc);
    return ;
}

void AESCipher::SetKey(const void *key,size_t size)
{
    /* 这里并未检测 key,size 的合法性,因为根据 openssl 对 AES_set_encrypt_key()
     * 的实现可以看出其内部做了检查.
     *
     * 是的,我们这里依赖 openssl 的实现,如果 openssl 实现更改,那么别忘了在这加个检测~
     */
    key_ptr_ = key;
    key_size_bits_ = (size << 3);
    return ;
}

void AESCipher::Encrypt(void *dst,const void *in)
{
    AES_encrypt(static_cast<const unsigned char*>(in),static_cast<unsigned char*>(dst),&ctx_);
    return ;
}

void AESCipher::Decrypt(void *dst,const void *in)
{
    AES_decrypt(static_cast<const unsigned char*>(in),static_cast<unsigned char*>(dst),&ctx_);
    return ;
}

unsigned AESCipher::GetBlockSize() const
{
    return kBlockSize;
}


