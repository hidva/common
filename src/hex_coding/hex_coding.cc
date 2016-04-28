#include "hex_coding.h"
#include "exception/errno_exception.h"
#include "common/inline_utils.h"

#include <string.h>

#define THROW_RANGE_EXCEPTION(used,provided) THROW(ENAMETOOLONG,"Used: %zu;Provided: %zu",used,provided)

namespace {

constexpr char hex_enc_table[] = {
    '0','1','2','3','4','5','6','7',
    '8','9','A','B','C','D','E','F'
};
#define xdigittochar_s(x) (hex_enc_table[x])

}

size_t HexEncode(char *dst,size_t max_size,const void *ptr,size_t size)
{
    size_t need_size = (size << 1);
    if (max_size < need_size)
        THROW_RANGE_EXCEPTION(need_size,max_size);

    char *optr = (dst);
    const unsigned char *iptr = static_cast<const unsigned char*>(ptr);
    for (size_t i = 0; i < size; ++i,optr += 2) {
        unsigned char high,low,tmp;
        tmp = iptr[i];
        high = (tmp & 0xF0U) >> 4;
        low = (tmp &0x0FU);

        optr[0] = xdigittochar_s(high);
        optr[1] = xdigittochar_s(low);
    }

    return need_size;
}

namespace {

// 用于将 16 进制字符转化为数字,如: hex_table['c'] == 12;
// 若输入不是合法的字符,则值为 -1.
constexpr signed char hex_table[] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,
    -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};
#define toxdigit(ch)    (hex_table[static_cast<unsigned char>(ch)])

}

size_t HexDecode(void *dst,size_t max_size,const char *ptr,size_t size)
{
    if (IsOdd(size))
        THROW(EINVAL,"Input Buffer Size Is Odd!size: %zu",size);

    size_t need_size = (size >> 1);
    if (max_size < need_size)
        THROW_RANGE_EXCEPTION(need_size,max_size);

    unsigned char *optr = static_cast<unsigned char*>(dst);
    for (size_t i = 0,j = 0; i < size; i += 2,++j) {
        signed char high_tmp,low_tmp;
        unsigned char high,low;
        high_tmp = toxdigit(ptr[i]);
        low_tmp = toxdigit(ptr[i + 1]);
        if (high_tmp == -1 || low_tmp == -1)
            THROW(EINVAL,"Invalid Char;%c%c",ptr[i],ptr[i + 1]);
        high = high_tmp;
        low = low_tmp;
        optr[j] = ((high << 4) | low);
    }

    return need_size;
}
