#ifndef ORG_PP_QQ_COMMON_COMMON_UTILS_H
#define ORG_PP_QQ_COMMON_COMMON_UTILS_H

#include <string.h>

#include <set>
#include <vector>
#include <iostream>
#include <random>
#include <algorithm>

#include "common/inline_utils.h"

/**
 * 按照 C++ 标准中定义的规则来比较 l1,s1 与 l2,s2 确定的字节区间.
 *
 * @param l1,s1,l2,s2 若 l1(或 l2) 为 nullptr,则等同于空.
 *
 * @return 与 memcmp() 一致.
 */
int MemCompare(const void *l1,size_t s1,const void *l2,size_t s2) noexcept;

inline int MemCompare(const void *l1,size_t s1,const char *l2) noexcept
{
    return MemCompare(l1,s1,l2,StrLen(l2));
}

inline int MemCompare(const char *l1,const char *l2) noexcept
{
    return MemCompare(l1,StrLen(l1),l2,StrLen(l2));
}

inline int MemCompare(const char *l1,const void *l2,size_t s2) noexcept
{
    return MemCompare(l1,StrLen(l1),l2,s2);
}

enum class LineTraverseReturn {
    kBreak = 0,
    kContinue,
};

/**
 * 在 [begin,end) 之间找到第一个不是 ch 的字符,并返回其指针.
 *
 * @return nullptr 表明 begin,end 之间全是 ch 字符.
 */
inline const char* FindFirstNotOf(const char *begin, const char * const end, char const ch) noexcept {
    for ( ; begin < end; ++begin)
        if (*begin != ch)
            return begin;
    return nullptr;
}

/**
 * 在 [begin,end) 之间找到第一个是 ch 的字符,并返回其指针.
 *
 * @return end 表明 begin,end 之间全不是 ch 字符.
 */
inline const char* FindFirstOf(const char *begin, const char * const end, char const ch) noexcept {
    ptrdiff_t sizes[] {0, end - begin};
    const void *ptr = memchr(begin,ch,sizes[begin < end]);
    const char *return_val [] {static_cast<const char*>(ptr), end};
    return return_val[ptr == nullptr];
}

template <typename F>
void SplitTraverse(const char *src, size_t size, char const sepa, F &&f) {
    const char * const src_end = src + size;

    for (const char *begin = src; begin = FindFirstNotOf(begin, src_end,sepa); ) {
        const char *end = FindFirstOf(begin + 1, src_end,sepa);

        if (f(begin, end - begin) == LineTraverseReturn::kBreak)
            break;

        begin = end + 1;
    }

    return ;
}

/**
 * 使用'\n'来分割 [src, src + size),对于非空的部分,顺序调用 f 来进行处理.
 *
 * @param f 其调用形式如下:
 *      // line_begin,line_size 存储着一行内容,并且一定非空.
 *      // line_begin,line_size 确定的存储区域中肯定不会含有 '\n',并且不保证 line_begin[line_size] == '\0'.
 *      //
 *      // 返回 kBreak,表明终止遍历;返回 kContinue 表明继续遍历.
 *      LineTraverseReturn f(const char *line_begin, size_t line_size);
 */
template <typename F>
void LineTraverse(const char *src, size_t size, F &&f) {
    SplitTraverse(src,size,'\n',std::forward<F>(f));
    return ;
}



template <typename F>
inline void LineTraverse(const char *src, F &&f) {
    LineTraverse(src, strlen(src), std::forward<F>(f));
    return ;
}

/**
 * 使用带权随机数算法在 begin,end 中随机选择一个元素,并返回该元素的迭代器.
 *
 * @param GetWeight 用于获取元素的权重.对于同一个元素来说,GetWeight() 可能会调用
 *  多次.调用形式如下:
 *
 *      // 注意权重的类型是 unsigned int 类型.
 *      weight_t GetWeight(elem) noexcept;
 */
template <typename Iter,typename F>
Iter GetWeightedRandom(Iter const begin, Iter const end, F GetWeight) {
#if 1
    thread_local std::mt19937_64 g_random_engine{std::random_device{}()};
#else
    std::random_device g_random_engine;
#endif

    using weight_t = unsigned int;
    using weight_sum_t = unsigned long long;

    weight_sum_t weight_sum = 0;
    for (Iter ptr = begin ; ptr != end; ++ptr)
        weight_sum += GetWeight(*ptr);

    if (weight_sum > 0) {
        std::uniform_int_distribution<weight_sum_t> random_gene(0U,weight_sum - 1);
        weight_sum_t random_weight = random_gene(g_random_engine);

        weight_sum_t partion_weight_sum = 0;
        for (Iter ptr = begin; ptr != end; ++ptr) {
            partion_weight_sum += GetWeight(*ptr);

            if (random_weight < partion_weight_sum)
                return ptr;
        }
    }

    return end;
}

template <typename Iter>
std::ostream& PrintArray(std::ostream &out, Iter first, Iter last) {
    out << '[';

    for (; first != last; ++first)
        out << *first << ',';

    out << ']';
    return out;
}

template <typename Type>
inline std::ostream& operator<<(std::ostream &out,const std::vector<Type> &v) {
    PrintArray(out, v.begin(), v.end());
    return out;
}

template <typename Type>
inline std::ostream& operator<<(std::ostream &out, const std::set<Type> &s) {
    PrintArray(out, s.begin(), s.end());
    return out;
}

/**
 * 从 begin 所指位置开始(包括 begin) 对 [first, last) 进行环回遍历,直至再一次遇到 begin 时
 * 终止遍历.
 *
 * @param first,last,begin [first, last) 必须不能为空, 而且 begin 属于 [first, last).
 *
 * @param f 其调用形式如下:
 *
 *      int f(Iter iter) noexcept;
 *  若返回 0,则表明继续遍历;返回 1,表明终止遍历.
 */
template <typename Iter, typename F>
void LoopbackTraverse(Iter first, Iter last, Iter begin, F f) noexcept {
    for (Iter ptr = begin; ; ) {
        int rc = f(ptr);
        if (rc == 1)
            break;

        ++ptr;
        if (ptr == last)
            ptr = first;

        if (ptr == begin)
            break;
    }
    return ;
}


struct MeteringIterator: public std::iterator<std::output_iterator_tag, void, void, void, void> {
public:
    MeteringIterator(size_t *times_ptr) noexcept :
        times(times_ptr) {
        *times = 0;
    }

    template <typename T>
    MeteringIterator& operator= (T &&elem) noexcept {
        ++*times;
        return *this;
    }

    MeteringIterator& operator* () noexcept {
        return *this;
    }

    MeteringIterator& operator++ () noexcept {
        return *this;
    }

    MeteringIterator operator++ (int) noexcept {
        return *this;
    }

public:
    size_t *times = nullptr;
};

template< class InputIt1, class InputIt2>
inline size_t GetSetIntersectionNumber(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2) {
    size_t times = 0;
    std::set_intersection(first1, last1, first2, last2, MeteringIterator{&times});
    return times;
}

// TODO(wangwei): RemoveIf() 需要单元测试
template <typename Iter, typename F>
Iter RemoveIf(Iter first, Iter last, F predicate) {
    for (; first != last; ++first) {
        Iter first_next = first;
        ++first_next;
        if (first_next == last)
            break;

        last = std::remove_if(
                    first_next, last,
                    [&] (const typename std::iterator_traits<Iter>::value_type &r_val) -> bool {
                        return predicate(*first, r_val);
                    }
        );
    }
    return last;
}

int GetUtf8LetterNumber(const char *src) noexcept;


#endif // ORG_PP_QQ_COMMON_COMMON_UTILS_H
