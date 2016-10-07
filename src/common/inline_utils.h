#ifndef ORG_PP_QQ_COMMON_COMMON_INLINE_UTILS_H
#define ORG_PP_QQ_COMMON_COMMON_INLINE_UTILS_H

#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <utility>

// 偶数
inline bool IsEven(unsigned long number) noexcept
{
    return (number & 0x01) == 0;
}

// 奇数
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

/**
 *  若 [p1,s1) 以 [p2,s2) 开始,则返回真. 该函数对空串(下以 NULL 称呼)的处理:
 *    p1,s1             p2,s2          return
 *     NULL              NULL           true
 *     NULL             NOTNUll         false
 *    NOTNULL            NULL           true
 *    NOTNULL           NOTNULL         实际求值.
 */
inline bool BeginWith(const void *p1,size_t s1,const void *p2,size_t s2) noexcept
{
    return s1 >= s2 && (memcmp(p1,p2,s2) == 0);
}

inline bool BeginWith(const void *p1,size_t s1,const void *p2) noexcept
{
    return BeginWith(p1,s1,p2,strlen(static_cast<const char*>(p2)));
}

inline bool BeginWith(const void *p1,const void *p2,size_t s2) noexcept
{
    return BeginWith(p1,strlen(static_cast<const char*>(p1)),p2,s2);
}

inline bool BeginWith(const void *p1,const void *p2) noexcept
{
    return BeginWith(p1,strlen(static_cast<const char*>(p1)),p2,strlen(static_cast<const char*>(p2)));
}


inline size_t StrLen(const char *s) noexcept
{
    return s == nullptr ? 0 : strlen(s);
}


struct InitHelper {

    template <typename F>
    InitHelper(F &&f) noexcept {
        (std::forward<F>(f))();
        return ;
    }

};

inline void Normalized(struct timeval &tv) noexcept {
    long a = tv.tv_usec / 1000000;
    long b = tv.tv_usec % 1000000;
    tv.tv_sec += a;
    tv.tv_usec = b;
    return ;
}


/* 计算 left - right;并将结果转化为纳秒.
 * @warning 当 left,right 之间相差太大时,可能会溢出.比如相差 560 年... T_T
 */
inline uint_fast64_t GetTimespecDiff(const struct timespec *left, const struct timespec *right) noexcept
{
    uint_fast64_t la = left->tv_sec;
    uint_fast64_t lb = left->tv_nsec;
    uint_fast64_t ra = right->tv_sec;
    uint_fast64_t rb = right->tv_nsec;
    return (la - ra) * 1000000000 + (lb - rb);
}

inline uint_fast64_t GetTimespecDiff(const struct timespec &left, const struct timespec &right) noexcept {
    return GetTimespecDiff(&left, &right);
}

#endif // ORG_PP_QQ_COMMON_COMMON_INLINE_UTILS_H
