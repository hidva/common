#include "cipher/block_cipher.h"
#include "exception/errno_exception.h"

#include <stdint.h>

/* TODO(wangwei): 未作抛出异常时的清理工作.
 *
 * 实际上,当代码逻辑正常时,异常是不会发生的;所以当异常发生时,一定是代码逻辑出问题了,
 */

ICipher::CipherResult BlockCipher::EncryptUpdate(void *dst,size_t max_size,const void *ptr,size_t size)
{
    if (status_ == kDecrypting)
        THROW(EINVAL,"当前正在处于解密过程中!不能调用加密函数.");

    if (status_ == kDontWork) {
        status_ = kEncrypting;
        mode_impl_->OnCipherBegin(this);
    }

    const size_t block_size = cipher_impl_->GetBlockSize();
    const size_t unused_size = size % block_size;
    const size_t used_size = size - unused_size;

    ICipher::CipherResult cipher_result;
    cipher_result.input_size = used_size;
    cipher_result.output_size = used_size;

    if (used_size > 0) {
        // used_size 一定是 block_size 的整数倍.

        if (IsOverlap(dst,max_size,ptr,size) && dst != ptr)
            THROW(EINVAL,"输出,输入重叠!");

        if (max_size < used_size)
            THROW(EINVAL,"输出缓冲区空间不足;需要: %zu;实际提供: %zu",used_size,max_size);

        mode_impl_->Update(this,dst,ptr,used_size);
    }

    return cipher_result;
}

ICipher::CipherResult BlockCipher::EncryptFinal(void *dst,size_t max_size,const void *ptr,size_t size)
{
    auto cipher_result = EncryptUpdate(dst,max_size,ptr,size);
    // Assert cipher_result.input_size == cipher_result.output_size 总是成立!

    const size_t unused_size = size - cipher_result.input_size;
    if (unused_size > 0) {
        // const size_t block_size = cipher_impl_->GetBlockSize();
        // Assert unused_size < block_size;

        /* 若 EncryptUpdate() 没有抛出异常,则表明对于 dst,max_size 与 ptr,size 的重叠性上,
         * 下列断言是成立的:
         *
         * 在 cipher_result.input_size != 0 时,ptr,size 与 dst,max_size 要么不重叠;要么
         * ptr == dst!
         */
        uint8_t *optr = static_cast<uint8_t*>(dst);
        const uint8_t *iptr = static_cast<const uint8_t*>(ptr);
        if (cipher_result.input_size > 0) {
            optr += cipher_result.input_size;
            iptr += cipher_result.input_size;
            max_size -= cipher_result.input_size;
            size = unused_size;
        } else if (IsOverlap(dst,max_size,ptr,size) && dst != ptr) {
            THROW(EINVAL,"输出,输入重叠!");
        }


        // Assert [optr,optr + max_size) 与 [iptr,iptr + size) 要么不相交;要么 optr == iptr;
        if (mode_impl_->IsNeedPadding()) {
            THROW(ENOTSUP,"现在还不支持填充呢!");
        } else {
            if (max_size < size)
                THROW(EINVAL,"输出缓冲区不足;需要: %zu;提供了 %zu",size,max_size);

            mode_impl_->Update(this,optr,iptr,size);
            cipher_result.input_size += size;
            cipher_result.output_size += size;
        }
    }

    mode_impl_->OnCipherEnd(this);
    status_ = kDontWork;

    return cipher_result;
}

ICipher::CipherResult BlockCipher::EncryptUpdateOutputSize(size_t input_size)
{
    const size_t block_size = cipher_impl_->GetBlockSize();
    const size_t used_size = input_size - (input_size % block_size);

    CipherResult cipher_result;
    cipher_result.input_size = used_size;
    cipher_result.output_size = used_size;
    return cipher_result;
}

ICipher::CipherResult BlockCipher::EncryptFinalOutputSize(size_t input_size)
{
    CipherResult cipher_result;

    if (!mode_impl_->IsNeedPadding()) {
        cipher_result.input_size = input_size;
        cipher_result.output_size = input_size;
    } else {
        const size_t block_size = cipher_impl_->GetBlockSize();
        const size_t unused_size = input_size % block_size;
        const size_t used_size = input_size - unused_size;
        cipher_result.input_size = unused_size > 0 ? used_size + block_size : used_size;
        cipher_result.output_size = cipher_result.input_size;
    }

    return cipher_result;
}


ICipher::CipherResult BlockCipher::DecryptUpdate(void *dst,size_t max_size,const void *ptr,size_t size)
{
    if (status_ == kEncrypting)
        THROW(EINVAL,"当前正在处于加密过程中!不能调用解密函数.");

    if (status_ == kDontWork) {
        status_ = kDecrypting;
        mode_impl_->OnCipherBegin(this);
    }

    const size_t block_size = cipher_impl_->GetBlockSize();
    const size_t unused_size = size % block_size;
    const size_t used_size = size - unused_size;

    ICipher::CipherResult cipher_result;
    cipher_result.input_size = used_size;
    cipher_result.output_size = used_size;

    if (used_size > 0) {
        // used_size 一定是 block_size 的整数倍.

        if (IsOverlap(dst,max_size,ptr,size) && dst != ptr)
            THROW(EINVAL,"输出,输入重叠!");

        if (max_size < used_size)
            THROW(EINVAL,"输出缓冲区空间不足;需要: %zu;实际提供: %zu",used_size,max_size);

        mode_impl_->Update(this,dst,ptr,used_size);
    }

    return cipher_result;
}

ICipher::CipherResult BlockCipher::DecryptFinal(void *dst,size_t max_size,const void *ptr,size_t size)
{
    auto cipher_result = DecryptUpdate(dst,max_size,ptr,size);

    const size_t unused_size = size - cipher_result.input_size;
    if (unused_size > 0) {
        // const size_t block_size = cipher_impl_->GetBlockSize();
        // Assert unused_size < block_size;

        /* 若 EncryptUpdate() 没有抛出异常,则表明对于 dst,max_size 与 ptr,size 的重叠性上,
         * 下列断言是成立的:
         *
         * 在 cipher_result.input_size != 0 时,ptr,size 与 dst,max_size 要么不重叠;要么
         * ptr == dst!
         */
        uint8_t *optr = static_cast<uint8_t*>(dst);
        const uint8_t *iptr = static_cast<const uint8_t*>(ptr);
        if (cipher_result.input_size > 0) {
            optr += cipher_result.input_size;
            iptr += cipher_result.input_size;
            max_size -= cipher_result.input_size;
            size = unused_size;
        } else if (IsOverlap(dst,max_size,ptr,size) && dst != ptr) {
            THROW(EINVAL,"输出,输入重叠!");
        }


        // Assert [optr,optr + max_size) 与 [iptr,iptr + size) 要么不相交;要么 optr == iptr;
        if (mode_impl_->IsNeedPadding()) {
            THROW(EINVAL,"需要填充的工作模式在解密时必须整块解密!");
        } else {
            if (max_size < size)
                THROW(EINVAL,"输出缓冲区不足;需要: %zu;提供了 %zu",size,max_size);

            mode_impl_->Update(this,optr,iptr,size);
            cipher_result.input_size += size;
            cipher_result.output_size += size;
        }
    }

    mode_impl_->OnCipherEnd(this);
    status_ = kDontWork;
    return cipher_result;
}

ICipher::CipherResult BlockCipher::DecryptUpdateOutputSize(size_t input_size)
{
    return EncryptUpdateOutputSize(input_size);
}

ICipher::CipherResult BlockCipher::DecryptFinalOutputSize(size_t input_size)
{
    CipherResult cipher_result;

    if (!mode_impl_->IsNeedPadding()) {
        cipher_result.input_size = input_size;
        cipher_result.output_size = input_size;
    } else {
        const size_t block_size = cipher_impl_->GetBlockSize();
        const size_t unused_size = input_size % block_size;

        if (unused_size > 0)
            THROW(EINVAL,"需要填充的工作模式在解密时必须整块解密!");

        cipher_result.input_size =  input_size;
        cipher_result.output_size = cipher_result.input_size;
    }

    return cipher_result;
}
