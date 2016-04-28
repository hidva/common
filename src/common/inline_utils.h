#ifndef ORG_PP_QQ_COMMON_COMMON_INLINE_UTILS_H
#define ORG_PP_QQ_COMMON_COMMON_INLINE_UTILS_H

#include <stddef.h>

inline bool IsEven(unsigned long number) noexcept
{
    return (number & 0x01) == 0;
}

inline bool IsOdd(unsigned long number) noexcept
{
    return !IsEven(number);
}

inline bool IsOverlap(const void *left,size_t size1,const void *right,size_t size2) noexcept
{
    const unsigned char *p1 = static_cast<const unsigned char*>(left);
    const unsigned char *p2 = static_cast<const unsigned char*>(right);
    return ((size1 == 0) || (size2 == 0)) ? false : ((p1 + size1) > p2 && p1 < (p2 + size2));
}


#endif // ORG_PP_QQ_COMMON_COMMON_INLINE_UTILS_H
