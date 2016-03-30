#ifndef ORG_PP_QQ_COMMON_HEX_CODING_HEX_CODING_H
#define ORG_PP_QQ_COMMON_HEX_CODING_HEX_CODING_H


#include <stddef.h>
#include "rrid/on_exception.h"


size_t HexEncode(char *buf,size_t max_size,const void *data,size_t size);
size_t HexDecode(void *data,size_t max_size,const char *buf,size_t size);

template <typename StringType>
StringType& HexEncode(StringType &str,const void *data,size_t size)
{
    size_t need_size = (size << 1);
    const size_t old_size = str.size();
    str.resize(old_size + need_size);
    ON_EXCEPTIN { str.resize(old_size);};

    HexEncode(const_cast<char*>(str.data() + old_size),need_size,data,size);
    return str;
}

template <typename StringType>
StringType& HexDecode(StringType &str,const char *buf,size_t size)
{
    size_t need_size = (size >> 1);
    const size_t old_size = str.size();
    str.resize(old_size + need_size);
    ON_EXCEPTIN {
        str.resize(old_size);
    };

    HexDecode(const_cast<char*>(str.data() + old_size),need_size,buf,size);
    return str;
}

template <typename StringType>
inline StringType HexEncode(const void *data,size_t size)
{
    StringType str;
    HexEncode(str,data,size);
    return str;
}

template <typename StringType>
StringType HexDecode(const char *buf,size_t size)
{
    StringType bytes;
    HexDecode(bytes,buf,size);
    return bytes;
}


#endif // ORG_PP_QQ_COMMON_HEX_CODING_HEX_CODING_H
