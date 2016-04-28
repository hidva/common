#ifndef ORG_PP_QQ_COMMON_CIPHER_ICIPHER_H
#define ORG_PP_QQ_COMMON_CIPHER_ICIPHER_H

#include <stddef.h>

#include "common/inline_utils.h"
#include "exception/errno_exception.h"


struct ICipher {

    struct CipherResult;

public:
    template <typename StringType>
    size_t EncryptUpdate(StringType &dst,const void *ptr,size_t size);

    template <typename StringType>
    size_t EncryptFinal(StringType &dst,const void *ptr,size_t size);

    template <typename StringType>
    size_t DecryptUpdate(StringType &dst,const void *ptr,size_t size);

    template <typename StringType>
    size_t DecryptFinal(StringType &dst,const void *ptr,size_t size);
public:

    virtual CipherResult EncryptUpdate(void *dst,size_t max_size,const void *ptr,size_t size) = 0;
    virtual CipherResult EncryptFinal(void *dst,size_t max_size,const void *ptr,size_t size) = 0;
    virtual CipherResult EncryptUpdateOutputSize(size_t input_size) = 0;
    virtual CipherResult EncryptFinalOutputSize(size_t input_size) = 0;

    virtual CipherResult DecryptFinal(void *dst,size_t max_size,const void *ptr,size_t size) = 0;
    virtual CipherResult DecryptUpdate(void *dst,size_t max_size,const void *ptr,size_t size) = 0;
    virtual CipherResult DecryptUpdateOutputSize(size_t input_size) = 0;
    virtual CipherResult DecryptFinalOutputSize(size_t input_size) = 0;
};

struct ICipher::CipherResult {
    size_t input_size;
    size_t output_size;
};

template <typename StringType>
size_t ICipher::EncryptUpdate(StringType &dst,const void *ptr,size_t size)
{
    // 此时若重叠,则在 dst.resize() 时可能覆盖 ptr,size 中的内容
    if (IsOverlap(dst.const_raw_data(),dst.capacity(),ptr,size))
        THROW(EINVAL,"输出,输入缓冲区重叠");

    CipherResult cipher_result = EncryptUpdateOutputSize(size);

    const size_t old_size = dst.size();
    dst.resize(old_size + cipher_result.output_size);
    ON_EXCEPTIN {
        dst.resize(old_size);
    };

    cipher_result = EncryptUpdate(dst.raw_data() + old_size,cipher_result.output_size,ptr,size);
    dst.resize(old_size + cipher_result.output_size);
    return cipher_result.input_size;
}

template <typename StringType>
size_t ICipher::EncryptFinal(StringType &dst,const void *ptr,size_t size)
{
    // 此时若重叠,则在 dst.resize() 时可能覆盖 ptr,size 中的内容
    if (IsOverlap(dst.const_raw_data(),dst.capacity(),ptr,size))
        THROW(EINVAL,"输出,输入缓冲区重叠");

    CipherResult cipher_result = EncryptFinalOutputSize(size);

    const size_t old_size = dst.size();
    dst.resize(old_size + cipher_result.output_size);
    ON_EXCEPTIN {
        dst.resize(old_size);
    };

    cipher_result = EncryptFinal(dst.raw_data() + old_size,cipher_result.output_size,ptr,size);
    dst.resize(old_size + cipher_result.output_size);
    return cipher_result.input_size;
}

template <typename StringType>
size_t ICipher::DecryptUpdate(StringType &dst,const void *ptr,size_t size)
{
    // 此时若重叠,则在 dst.resize() 时可能覆盖 ptr,size 中的内容
    if (IsOverlap(dst.const_raw_data(),dst.capacity(),ptr,size))
        THROW(EINVAL,"输出,输入缓冲区重叠");

    CipherResult cipher_result = DecryptUpdateOutputSize(size);

    const size_t old_size = dst.size();
    dst.resize(old_size + cipher_result.output_size);
    ON_EXCEPTIN {
        dst.resize(old_size);
    };

    cipher_result = DecryptUpdate(dst.raw_data() + old_size,cipher_result.output_size,ptr,size);
    dst.resize(old_size + cipher_result.output_size);
    return cipher_result.input_size;
}

template <typename StringType>
size_t ICipher::DecryptFinal(StringType &dst,const void *ptr,size_t size)
{
    // 此时若重叠,则在 dst.resize() 时可能覆盖 ptr,size 中的内容
    if (IsOverlap(dst.const_raw_data(),dst.capacity(),ptr,size))
        THROW(EINVAL,"输出,输入缓冲区重叠");

    CipherResult cipher_result = DecryptFinalOutputSize(size);

    const size_t old_size = dst.size();
    dst.resize(old_size + cipher_result.output_size);
    ON_EXCEPTIN {
        dst.resize(old_size);
    };

    cipher_result = DecryptFinal(dst.raw_data() + old_size,cipher_result.output_size,ptr,size);
    dst.resize(old_size + cipher_result.output_size);
    return cipher_result.input_size;
}

#endif // ORG_PP_QQ_COMMON_CIPHER_ICIPHER_H
