#include "cipher/ctr_cipher_mode.h"
#include "cipher/memxor.h"

bool CTRCipherMode::IsNeedPadding() const noexcept
{
    return false;
}

void CTRCipherMode::OnCipherBegin(BlockCipher *block_cipher)
{
    BlockCipherImpl *cipher = block_cipher->GetCipherImpl();

    auto block_size = cipher->GetBlockSize();
    if (block_size > kBlockSizeMax)
        THROW(EINVAL,"当前正在使用的块加密算法支持的数据块过长;BlockSizeMax: %zu;BlockSize: %zu",kBlockSizeMax,block_size);

    counter_impl_->Begin(next_counter_,block_size);
    cipher->InitMode(BlockCipherImpl::kEncryptMode);
    return ;
}

void CTRCipherMode::Update(BlockCipher *block_cipher,void *dst,const void *src,size_t size)
{
    BlockCipherImpl *cipher = block_cipher->GetCipherImpl();
    auto const block_size = cipher->GetBlockSize();
    // Assert: block_size <= kBlockSizeMax;

    if (size < block_size) {
        // 根据 Update() 的语义,此时 Update() 是在最后一次加密/解密过程中,所以没必要
        // 再更新 next_counter_ 的值了,也就是说可以直接使用 next_counter_ 的值.
        cipher->Encrypt(next_counter_,next_counter_);
        if (src != dst)
            Memxor(dst,src,next_counter_,size);
        else
            Memxor(dst,next_counter_,size);
    } else {
        // Assert: size % block_size == 0;
        unsigned char *optr = static_cast<unsigned char*>(dst);
        const unsigned char *iptr = static_cast<const unsigned char*>(src);

        if (optr == iptr) {
            unsigned char counter_buffer[kBlockSizeMax];
            while (size > 0) {
                memcpy(counter_buffer_,next_counter_,block_size);
                counter_impl_->Inc(next_counter_,block_size);

                cipher->Encrypt(counter_buffer,counter_buffer);
                Memxor(optr,counter_buffer,block_size);

                iptr += block_size;
                optr += block_size;
                size -= block_size;
            }
        } else {
            while (size > 0) {
                memcpy(optr,next_counter_,block_size);
                counter_impl_->Inc(next_counter_,block_size);

                cipher->Encrypt(optr,optr);
                Memxor(optr,iptr,block_size);

                optr += block_size;
                iptr += block_size;
                size -= block_size;
            }
        }
    }

    return ;
}

void CTRCipherMode::OnCipherEnd(BlockCipher *block_cipher)
{
    return ;
}
