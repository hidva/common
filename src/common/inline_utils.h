#ifndef ORG_PP_QQ_COMMON_COMMON_INLINE_UTILS_H
#define ORG_PP_QQ_COMMON_COMMON_INLINE_UTILS_H

inline bool IsEven(unsigned long number) noexcept
{
    return (number & 0x01) == 0;
}

inline bool IsOdd(unsigned long number) noexcept
{
    return !IsEven(number);
}


#endif // ORG_PP_QQ_COMMON_COMMON_INLINE_UTILS_H
