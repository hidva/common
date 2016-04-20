#ifndef ORG_PP_QQ_COMMON_STRING_FB_STRING_BASIC_H
#define ORG_PP_QQ_COMMON_STRING_FB_STRING_BASIC_H

// 此处实现了参考了 Facebook/folly 中 FBString 的实现


/* 在实现过程中,注意以下问题:

**注意** 延迟加'\0'的问题.
**注意** expand_noinit() 不再会追加0.
**注意** raw_data(),const_raw_data();data(),4142041420412021514625362


    1. 在调用 data() 时,根据情况转化为 cdata(),raw_data();
    2. 在调用 expand_noinit() 时需要注意末尾不再保证 0!
    3. begin(),end() 与 cbegin(),cend();


 */

#include <string>

#if defined(__GNUC__) && __GNUC__ >= 4
#define FBSTRING_LIKELY(x)   (__builtin_expect((x), 1))
#define FBSTRING_UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define FBSTRING_LIKELY(x)   (x)
#define FBSTRING_UNLIKELY(x) (x)
#endif

namespace fbstring_detail {

// Pod,POD 结构;

template <class InIt, class OutIt>
inline OutIt
CopyN(InIt b,typename std::iterator_traits<InIt>::difference_type n,OutIt d)
{
    for (; n != 0; --n, ++b, ++d)
        *d = *b;
    return d;
}

template <class Pod, class T>
inline void
PodFill(Pod* b, Pod* e, T c)
{
    if (sizeof(T) == 1) {
        memset(b, c, e - b);
    } else {
        auto const ee = b + ((e - b) & ~7u);
        for (; b != ee; b += 8) {
            b[0] = c;
            b[1] = c;
            b[2] = c;
            b[3] = c;
            b[4] = c;
            b[5] = c;
            b[6] = c;
            b[7] = c;
        }

        for (; b != e; ++b)
          *b = c;
    }
    return ;
}

template <class Pod>
inline void
pod_copy(const Pod* b, const Pod* e, Pod* d)
{
    memcpy(d, b, (e - b) * sizeof(Pod));
    return ;
}

template <class Pod>
inline void
pod_move(const Pod* b, const Pod* e, Pod* d)
{
    memmove(d, b, (e - b) * sizeof(*b));
    return ;
}

} // namespace fbstring_detail


template <typename E,
          typename Storage,
          typename T = std::char_traits<E>,
          typename A = std::allocator<E> >
class FBStringBasic {

public:
    enum class AcquireMallocatedString {};

    typedef T traits_type;
    typedef typename traits_type::char_type value_type;
    typedef A allocator_type;
    typedef typename A::size_type size_type;
    typedef typename A::difference_type difference_type;

    typedef typename A::reference reference;
    typedef typename A::const_reference const_reference;
    typedef typename A::pointer pointer;
    typedef typename A::const_pointer const_pointer;

    typedef E* iterator;
    typedef const E* const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

public:
    static constexpr size_type npos;                     // = size_type(-1)

public:
    inline FBStringBasic() noexcept;
    inline FBStringBasic(const FBStringBasic &);
    inline FBStringBasic(FBStringBasic &&);

    inline explicit FBStringBasic(const A &) noexcept;
    inline FBStringBasic(const FBStringBasic &,size_type pos,size_type n = npos,const A & = A{});
    inline FBStringBasic(const value_type* s, const A& = A());
    inline FBStringBasic(const value_type* s, size_type n, const A& = A());
    inline FBStringBasic(size_type n, value_type c, const A& /*a*/ = A());

    template <typename InIt>
    inline FBStringBasic(InIt begin, InIt end,
        typename std::enable_if<
            !std::is_same<typename std::remove_const<InIt>::type,value_type*>::value,
            const A>::type & = A());

    inline FBStringBasic(const value_type* b, const value_type* e);
    inline FBStringBasic(value_type *s, size_type n, size_type c,AcquireMallocatedString a);
    inline FBStringBasic(std::initializer_list<value_type> il);

    inline ~FBStringBasic();

    FBStringBasic& operator=(const FBStringBasic& lhs);
    FBStringBasic& operator=(FBStringBasic&& goner) noexcept;
    inline FBStringBasic& operator=(const value_type* s);
    FBStringBasic& operator=(value_type c);
    inline FBStringBasic& operator=(std::initializer_list<value_type> il);

    inline iterator begin();
    inline const_iterator begin() const;
    inline const_iterator cbegin() const;

    inline iterator end();
    inline const_iterator end() const;
    inline const_iterator cend() const;

    inline reverse_iterator rbegin();
    inline const_reverse_iterator rbegin() const;
    inline const_reverse_iterator crbegin() const;

    inline reverse_iterator rend();
    inline const_reverse_iterator rend() const;
    inline const_reverse_iterator crend() const;

    inline value_type& front();
    inline const value_type& front() const;
    inline const value_type& cfront() const;

    inline value_type& back();
    inline const value_type& back() const;
    inline const value_type& cback() const;

    inline void pop_back();
    inline void push_back(const value_type c);

    inline size_type size() const;
    inline size_type length() const;
    inline size_type max_size() const;
    inline size_type capacity() const;

    void resize(const size_type n, const value_type c = value_type());
    inline void reserve(size_type res_arg = 0);
    inline void shrink_to_fit();
    inline void clear();

    inline bool empty() const;

    inline reference operator[](size_type pos);
    inline const_reference operator[](size_type pos) const;

    inline reference at(size_type n);
    inline const_reference at(size_type n) const;
    inline const_reference cat(size_type n) const;

    inline FBStringBasic& operator+=(const FBStringBasic& str);
    inline FBStringBasic& operator+=(const value_type* s);
    inline FBStringBasic& operator+=(const value_type c);
    inline FBStringBasic& operator+=(std::initializer_list<value_type> il);

    inline FBStringBasic& append(const FBStringBasic& str);
    inline FBStringBasic& append(const FBStringBasic& str, const size_type pos,size_type n);
    FBStringBasic& append(const value_type* s, size_type n);
    inline FBStringBasic& append(const value_type* s);
    inline FBStringBasic& append(size_type n, value_type c);
    inline FBStringBasic& append(std::initializer_list<value_type> il);
    template<class InputIterator>
    inline FBStringBasic& append(InputIterator first, InputIterator last);

    inline FBStringBasic& assign(const FBStringBasic& str);
    inline FBStringBasic& assign(FBStringBasic&& str);
    inline FBStringBasic& assign(const FBStringBasic& str, const size_type pos,size_type n);
    FBStringBasic& assign(const value_type* s, const size_type n);
    inline FBStringBasic& assign(const value_type* s);
    inline FBStringBasic& assign(std::initializer_list<value_type> il);
    template <class ItOrLength, class ItOrChar>
    inline FBStringBasic& assign(ItOrLength first_or_n, ItOrChar last_or_c);

    inline FBStringBasic& insert(size_type pos1, const FBStringBasic& str);
    inline FBStringBasic& insert(size_type pos1, const FBStringBasic& str,size_type pos2, size_type n);
    inline FBStringBasic& insert(size_type pos, const value_type* s, size_type n);
    inline FBStringBasic& insert(size_type pos, const value_type* s);
    inline FBStringBasic& insert(size_type pos, size_type n, value_type c);
    inline iterator insert(const_iterator p, const value_type c);
    template <class ItOrLength, class ItOrChar>
    inline iterator insert(const_iterator p, ItOrLength first_or_n, ItOrChar last_or_c);
    inline iterator insert(const_iterator p, std::initializer_list<value_type> il);

    inline FBStringBasic& erase(size_type pos = 0, size_type n = npos);
    inline iterator erase(iterator position);
    inline iterator erase(iterator first, iterator last);

    inline FBStringBasic& replace(size_type pos1, size_type n1,const FBStringBasic& str);
    inline FBStringBasic& replace(size_type pos1, size_type n1,const FBStringBasic& str,size_type pos2, size_type n2);
    inline FBStringBasic& replace(size_type pos, size_type n1, const value_type* s);
    template <class StrOrLength, class NumOrChar>
    inline FBStringBasic& replace(size_type pos, size_type n1,StrOrLength s_or_n2, NumOrChar n_or_c);
    inline FBStringBasic& replace(iterator i1, iterator i2, const FBStringBasic& str);\
    inline FBStringBasic& replace(iterator i1, iterator i2, const value_type* s);
    template <class T1, class T2>
    inline FBStringBasic& replace(iterator i1, iterator i2,T1 first_or_n_or_s, T2 last_or_c_or_n);

    inline size_type copy(value_type* s, size_type n, size_type pos = 0) const;

    inline void swap(FBStringBasic& rhs);

    inline const value_type* c_str() const;

    inline value_type* data();
    inline const value_type* data() const;
    inline const value_type* cdata() const;

    inline value_type* raw_data();
    inline const value_type* raw_data() const;
    inline const value_type* const_raw_data() const;

    inline allocator_type get_allocator() const;

    inline size_type find(const FBStringBasic& str, size_type pos = 0) const;
    size_type find(const value_type* needle, const size_type pos,const size_type nsize) const;
    inline size_type find(const value_type* s, size_type pos = 0) const ;
    inline size_type find (value_type c, size_type pos = 0) const;

    inline size_type rfind(const FBStringBasic& str, size_type pos = npos) const;
    size_type rfind(const value_type* s, size_type pos, size_type n) const;
    inline size_type rfind(const value_type* s, size_type pos = npos) const;
    inline size_type rfind(value_type c, size_type pos = npos) const;

    inline size_type find_first_of(const FBStringBasic& str, size_type pos = 0) const;
    size_type find_first_of(const value_type* s,size_type pos, size_type n) const;
    inline size_type find_first_of(const value_type* s, size_type pos = 0) const;
    inline size_type find_first_of(value_type c, size_type pos = 0) const;

    inline size_type find_last_of (const FBStringBasic& str,size_type pos = npos) const;
    size_type find_last_of (const value_type* s, size_type pos,size_type n) const;
    inline size_type find_last_of (const value_type* s,size_type pos = npos) const;
    inline size_type find_last_of (value_type c, size_type pos = npos) const;

    inline size_type find_first_not_of(const FBStringBasic& str,size_type pos = 0) const;
    size_type find_first_not_of(const value_type* s, size_type pos,size_type n) const;
    inline size_type find_first_not_of(const value_type* s,size_type pos = 0) const;
    inline size_type find_first_not_of(value_type c, size_type pos = 0) const;

    inline size_type find_last_not_of(const FBStringBasic& str,size_type pos = npos) const;
    size_type find_last_not_of(const value_type* s, size_type pos,size_type n) const;
    inline size_type find_last_not_of(const value_type* s,size_type pos = npos) const;
    inline size_type find_last_not_of (value_type c, size_type pos = npos) const;

    inline FBStringBasic substr(size_type pos = 0, size_type n = npos) const &;
    inline FBStringBasic substr(size_type pos = 0, size_type n = npos) &&;

    inline int compare(const FBStringBasic& str) const;
    inline int compare(size_type pos1, size_type n1,const FBStringBasic& str) const;
    inline int compare(size_type pos1, size_type n1,const value_type* s) const;
    inline int compare(size_type pos1, size_type n1,const value_type* s, size_type n2) const;
    inline int compare(size_type pos1, size_type n1,const FBStringBasic& str,size_type pos2, size_type n2) const;
    inline int compare(const value_type* s) const;

private:
    template <int i>
    class Selector {};

    // insert()
    iterator insertImplDiscr(const_iterator i,size_type n, value_type c, Selector<1>);

    template<class InputIter>
    iterator insertImplDiscr(const_iterator i,InputIter b, InputIter e, Selector<0>);

    template <class FwdIterator>
    iterator insertImpl(const_iterator i,FwdIterator s1,FwdIterator s2,std::forward_iterator_tag);

    template <class InputIterator>
    iterator insertImpl(const_iterator i,InputIterator b, InputIterator e,std::input_iterator_tag);

    // replace
    inline FBStringBasic& replaceImplDiscr(iterator i1, iterator i2,const value_type* s, size_type n,Selector<2>);

    FBStringBasic& replaceImplDiscr(iterator i1, iterator i2,size_type n2, value_type c, Selector<1>);

    template <class InputIter>
    inline FBStringBasic& replaceImplDiscr(iterator i1, iterator i2,InputIter b, InputIter e,Selector<0>);

    template <class FwdIterator>
    inline bool replaceAliased(iterator /* i1 */,iterator /* i2 */,FwdIterator /* s1 */,FwdIterator /* s2 */,std::false_type);

    template <class FwdIterator>
    bool replaceAliased(iterator i1, iterator i2,FwdIterator s1, FwdIterator s2, std::true_type);

    template <class FwdIterator>
    void replaceImpl(iterator i1, iterator i2,FwdIterator s1, FwdIterator s2, std::forward_iterator_tag);

    template <class InputIterator>
    inline void replaceImpl(iterator i1, iterator i2,InputIterator b, InputIterator e, std::input_iterator_tag);

private:
    Storage store_;
};

template <typename E, class T, class A, class S>
inline FBStringBasic<E, T, A, S>
operator+(const FBStringBasic<E, T, A, S>& lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    FBStringBasic<E, T, A, S> result;
    result.reserve(lhs.size() + rhs.size());
    result.append(lhs).append(rhs);
    return std::move(result);
}

template <typename E, class T, class A, class S>
inline FBStringBasic<E, T, A, S>
operator+(FBStringBasic<E, T, A, S>&& lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return std::move(lhs.append(rhs));
}

template <typename E, class T, class A, class S>
inline FBStringBasic<E, T, A, S>
operator+(const FBStringBasic<E, T, A, S>& lhs,FBStringBasic<E, T, A, S>&& rhs)
{
    if (rhs.capacity() >= lhs.size() + rhs.size())
        return std::move(rhs.insert(0, lhs));
    auto const& rhsC = rhs;
    return lhs + rhsC;
}

template <typename E, class T, class A, class S>
inline FBStringBasic<E, T, A, S>
operator+(FBStringBasic<E, T, A, S>&& lhs,FBStringBasic<E, T, A, S>&& rhs)
{
    return std::move(lhs.append(rhs));
}

template <typename E, class T, class A, class S>
inline FBStringBasic<E, T, A, S>
operator+(const E* lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    FBStringBasic<E, T, A, S> result;
    const auto len = FBStringBasic<E, T, A, S>::traits_type::length(lhs);
    result.reserve(len + rhs.size());
    result.append(lhs, len).append(rhs);
    return result;
}

template <typename E, class T, class A, class S>
inline FBStringBasic<E, T, A, S>
operator+(const E* lhs,FBStringBasic<E, T, A, S>&& rhs)
{
    const auto len = FBStringBasic<E, T, A, S>::traits_type::length(lhs);
    if (rhs.capacity() >= len + rhs.size()) {
        rhs.insert(rhs.begin(), lhs, lhs + len);
        return rhs;
    }

    FBStringBasic<E, T, A, S> result;
    result.reserve(len + rhs.size());
    result.append(lhs, len).append(rhs);
    return result;
}

template <typename E, class T, class A, class S>
inline FBStringBasic<E, T, A, S>
operator+(E lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    FBStringBasic<E, T, A, S> result;
    result.reserve(1 + rhs.size());
    result.push_back(lhs);
    result.append(rhs);
    return result;
}

template <typename E, class T, class A, class S>
inline FBStringBasic<E, T, A, S>
operator+(E lhs,FBStringBasic<E, T, A, S>&& rhs)
{
    if (rhs.capacity() > rhs.size()) {
        rhs.insert(rhs.begin(), lhs);
        return rhs;
    }

    auto const& rhsC = rhs;
    return lhs + rhsC;
}

template <typename E, class T, class A, class S>
inline FBStringBasic<E, T, A, S>
operator+(const FBStringBasic<E, T, A, S>& lhs,const E* rhs)
{
    typedef typename FBStringBasic<E, T, A, S>::size_type size_type;
    typedef typename FBStringBasic<E, T, A, S>::traits_type traits_type;

    FBStringBasic<E, T, A, S> result;
    const size_type len = traits_type::length(rhs);
    result.reserve(lhs.size() + len);
    result.append(lhs).append(rhs, len);
    return result;
}

template <typename E, class T, class A, class S>
inline FBStringBasic<E, T, A, S>
operator+(FBStringBasic<E, T, A, S>&& lhs,const E* rhs)
{
    return std::move(lhs += rhs);
}

template <typename E, class T, class A, class S>
inline FBStringBasic<E, T, A, S>
operator+(const FBStringBasic<E, T, A, S>& lhs,E rhs)
{
    FBStringBasic<E, T, A, S> result;
    result.reserve(lhs.size() + 1);
    result.append(lhs);
    result.push_back(rhs);
    return result;
}

template <typename E, class T, class A, class S>
inline FBStringBasic<E, T, A, S>
operator+(FBStringBasic<E, T, A, S>&& lhs,E rhs)
{
    return std::move(lhs += rhs);
}

template <typename E, class T, class A, class S>
inline bool
operator==(const FBStringBasic<E, T, A, S>& lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
}

template <typename E, class T, class A, class S>
inline bool
operator==(const typename FBStringBasic<E, T, A, S>::value_type* lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return rhs == lhs;
}

template <typename E, class T, class A, class S>
inline bool
operator==(const FBStringBasic<E, T, A, S>& lhs,const typename FBStringBasic<E, T, A, S>::value_type* rhs)
{
    return lhs.compare(rhs) == 0;
}

template <typename E, class T, class A, class S>
inline bool
operator!=(const FBStringBasic<E, T, A, S>& lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return !(lhs == rhs);
}

template <typename E, class T, class A, class S>
inline bool
operator!=(const typename FBStringBasic<E, T, A, S>::value_type* lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return !(lhs == rhs);
}

template <typename E, class T, class A, class S>
inline bool
operator!=(const FBStringBasic<E, T, A, S>& lhs,const typename FBStringBasic<E, T, A, S>::value_type* rhs)
{
    return !(lhs == rhs);
}

template <typename E, class T, class A, class S>
inline bool
operator<(const FBStringBasic<E, T, A, S>& lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return lhs.compare(rhs) < 0;
}

template <typename E, class T, class A, class S>
inline bool
operator<(const FBStringBasic<E, T, A, S>& lhs,const typename FBStringBasic<E, T, A, S>::value_type* rhs)
{
    return lhs.compare(rhs) < 0;
}

template <typename E, class T, class A, class S>
inline bool
operator<(const typename FBStringBasic<E, T, A, S>::value_type* lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return rhs.compare(lhs) > 0;
}

template <typename E, class T, class A, class S>
inline bool
operator>(const FBStringBasic<E, T, A, S>& lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return rhs < lhs;
}

template <typename E, class T, class A, class S>
inline bool
operator>(const FBStringBasic<E, T, A, S>& lhs,const typename FBStringBasic<E, T, A, S>::value_type* rhs)
{
    return rhs < lhs;
}

template <typename E, class T, class A, class S>
inline bool
operator>(const typename FBStringBasic<E, T, A, S>::value_type* lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return rhs < lhs;
}

template <typename E, class T, class A, class S>
inline bool
operator<=(const FBStringBasic<E, T, A, S>& lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return !(rhs < lhs);
}

template <typename E, class T, class A, class S>
inline bool
operator<=(const FBStringBasic<E, T, A, S>& lhs,const typename FBStringBasic<E, T, A, S>::value_type* rhs)
{
    return !(rhs < lhs);
}

template <typename E, class T, class A, class S>
inline bool
operator<=(const typename FBStringBasic<E, T, A, S>::value_type* lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return !(rhs < lhs);
}

template <typename E, class T, class A, class S>
inline bool
operator>=(const FBStringBasic<E, T, A, S>& lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return !(lhs < rhs);
}

template <typename E, class T, class A, class S>
inline bool
operator>=(const FBStringBasic<E, T, A, S>& lhs,const typename FBStringBasic<E, T, A, S>::value_type* rhs)
{
    return !(lhs < rhs);
}

template <typename E, class T, class A, class S>
inline bool
operator>=(const typename FBStringBasic<E, T, A, S>::value_type* lhs,const FBStringBasic<E, T, A, S>& rhs)
{
    return !(lhs < rhs);
}

template <typename E, class T, class A, class S>
inline void
swap(FBStringBasic<E, T, A, S>& lhs, FBStringBasic<E, T, A, S>& rhs)
{
    lhs.swap(rhs);
    return ;
}

template <typename E1, class T, class A, class S>
const typename FBStringBasic<E1, T, A, S>::size_type
FBStringBasic<E1, T, A, S>::npos = static_cast<typename FBStringBasic<E1, T, A, S>::size_type>(-1);


template <typename E, class S, class T, class A>
FBStringBasic<E,S,T,A>::FBStringBasic() noexcept:
    FBStringBasic(A{})
{
}

template <typename E, class S, class T, class A>
FBStringBasic<E,S,T,A>::FBStringBasic(const FBStringBasic &other):
    store_(other.store_)
{
}

template <typename E, class S, class T, class A>
FBStringBasic<E,S,T,A>::FBStringBasic(FBStringBasic &&other):
    store_(std::move(other.store_))
{
}

template <typename E, class S, class T, class A>
FBStringBasic<E,S,T,A>::FBStringBasic(const A &) noexcept
{
}

template <typename E, class S, class T, class A>
FBStringBasic<E,S,T,A>::FBStringBasic(const FBStringBasic &str,size_type pos,size_type n = npos,const A & = A{})
{
    assign(str,pos,n);
}

template <typename E, class S, class T, class A>
FBStringBasic<E,S,T,A>::FBStringBasic(const value_type* s, const A& = A()):
    store_(s,s != nullptr ? traits_type::length(s) : 0)
{
}

template <typename E, class S, class T, class A>
FBStringBasic<E,S,T,A>::FBStringBasic(const value_type* s, size_type n, const A& = A()):
    store_(s,n)
{
}

template <typename E, class S, class T, class A>
FBStringBasic<E,S,T,A>::FBStringBasic(size_type n, value_type c, const A& /*a*/ = A())
{
    auto const ptr = store_.expand_noinit(n);
    fbstring_detail::PodFill(ptr, ptr + n, c);
}

template <typename E, class S, class T, class A> template <typename InIt>
FBStringBasic<E,S,T,A>::FBStringBasic(InIt begin, InIt end,
    typename std::enable_if<
        !std::is_same<typename std::remove_const<InIt>::type,value_type*>::value,
        const A>::type & = A())
{
    assign(begin, end);
}

template <typename E, class S, class T, class A>
FBStringBasic<E,S,T,A>::FBStringBasic(const value_type* b, const value_type* e):
    store_(b,e - b)
{
}

template <typename E, class S, class T, class A>
FBStringBasic<E,S,T,A>::FBStringBasic(value_type *s, size_type n, size_type c,AcquireMallocatedString a):
    store_(s,n,c,a)
{
}

template <typename E, class S, class T, class A>
FBStringBasic<E,S,T,A>::FBStringBasic(std::initializer_list<value_type> il)
{
    assign(il.begin(),il.end());
}

template <typename E, class S, class T, class A>
FBStringBasic<E,S,T,A>::~FBStringBasic()
{
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::operator=(const FBStringBasic& lhs) -> FBStringBasic&
{
    if (FBSTRING_UNLIKELY(&lhs == this))
        return *this;

    auto const old_size = size();
    auto const src_size = lhs.size();
    if (capacity() >= src_size && !store_.isShared()) {
        // great, just copy the contents
        if (old_size < src_size)
            store_.expand_noinit(src_size - old_size);
        else
            store_.shrink(old_size - src_size);

        auto src_data = lhs.const_raw_data();
        fbstring_detail::pod_copy(src_data, src_data + src_size, store_.mutable_data());
    } else {
        FBStringBasic(lhs).swap(*this);
    }
    return *this;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::operator=(FBStringBasic&& goner) noexcept -> FBStringBasic&
{
    if (FBSTRING_UNLIKELY(&goner == this))
        return *this;

    this->~FBStringBasic();
    new(&store_) S(std::move(goner.store_));
    return *this;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::operator=(const value_type* s) -> FBStringBasic&
{
    return assign(s);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::operator=(value_type c) -> FBStringBasic&
{
    if (empty()) {
        store_.expand_noinit(1);
    } else if (store_.isShared()) {
         FBStringBasic(1, c).swap(*this);
         return *this;
    } else {
        store_.shrink(size() - 1);
    }

    front() = c;
    return *this;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::operator=(std::initializer_list<value_type> il) -> FBStringBasic&
{
    return assign(il.begin(),il.end());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::begin() ->  FBStringBasic::iterator
{
    return store_.mutable_data();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::begin() const -> FBStringBasic::const_iterator
{
    return store_.data();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::cbegin() const -> FBStringBasic::const_iterator
{
    return begin();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::end() -> FBStringBasic::iterator
{
    return store_.mutable_data() + store_.size();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::end() const -> FBStringBasic::const_iterator
{
    return store_.data() + store_.size();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::cend() const -> FBStringBasic::const_iterator
{
    return end();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::rbegin() -> FBStringBasic::reverse_iterator
{
    return reverse_iterator(end());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::rbegin() const -> FBStringBasic::const_reverse_iterator
{
    return const_reverse_iterator(end());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::crbegin() const -> FBStringBasic::const_reverse_iterator
{
    return rbegin();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::rend() -> FBStringBasic::reverse_iterator
{
    return reverse_iterator(begin());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::rend() const -> FBStringBasic::const_reverse_iterator
{
    return const_reverse_iterator(begin());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::crend() const -> FBStringBasic::const_reverse_iterator
{
    return rend();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::front() -> FBStringBasic::value_type&
{
    /* 这里不使用 *begin();主要是考虑到 store_ 可能采用延迟加'\0'的问题,而且 begin() 是
     * 不需要加'\0'的,所以在为空串的时候,*begin() 可能会取到一个随机值之类的.
     *
     * 忽然又发现了标准里定义当为空串时,front() 未定义所以这里也不需要其他的了.
     */
    return *begin();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::front() const -> const FBStringBasic::value_type&
{
    return *begin();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::cfront() const -> const FBStringBasic::value_type&
{
    return front();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::back() -> FBStringBasic::value_type&
{
    return *(end() - 1);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::back() const -> const FBStringBasic::value_type&
{
    return *(end() - 1);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::cback() const -> const FBStringBasic::value_type&
{
    return back();
}

template <typename E, class S, class T, class A>
void FBStringBasic<E,S,T,A>::pop_back()
{
    store_.shrink(1);
    return ;
}

template <typename E, class S, class T, class A>
void FBStringBasic<E,S,T,A>::push_back(const value_type c)
{
    store_.push_back(c);
    return ;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::size() const -> size_type
{
    return store_.size();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::length() const -> size_type
{
    return size();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::max_size() const -> size_type
{
    return std::numeric_limits<size_type>::max();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::capacity() const -> size_type
{
    return store_.capacity();
}

template <typename E, class S, class T, class A>
void FBStringBasic<E,S,T,A>::resize(const size_type n, const value_type c)
{
    auto size = this->size();
    if (n <= size) {
        store_.shrink(size - n);
    } else {
      auto const delta = n - size;
      auto ptr = store_.expand_noinit(delta);
      fbstring_detail::PodFill(ptr, ptr + delta, c);
    }
    return ;
}

template <typename E, class S, class T, class A>
void FBStringBasic<E,S,T,A>::reserve(size_type res_arg)
{
    store_.reserve(res_arg);
    return ;
}

template <typename E, class S, class T, class A>
void FBStringBasic<E,S,T,A>::shrink_to_fit()
{
    if (capacity() < size() * 3 / 2)
        return ;
    FBStringBasic(cbegin(), cend()).swap(*this);
    return ;
}

template <typename E, class S, class T, class A>
void FBStringBasic<E,S,T,A>::clear()
{
    resize(0);
    return ;
}

template <typename E, class S, class T, class A>
bool FBStringBasic<E,S,T,A>::empty() const
{
    return size() == 0;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::operator[](size_type pos) -> reference
{
    return *(begin() + pos);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::operator[](size_type pos) const -> const_reference
{
    return *(begin() + pos);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::at(size_type n) -> reference
{
    return (*this)[n];
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::at(size_type n) const -> const_reference
{
    return (*this)[n];
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::cat(size_type n) const -> const_reference
{
    return at(n);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::operator+=(const FBStringBasic& str) -> FBStringBasic&
{
    return append(str);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::operator+=(const value_type* s) -> FBStringBasic&
{
    return append(s);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::operator+=(const value_type c) -> FBStringBasic&
{
    push_back(c);
    return *this;
}


template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::operator+=(std::initializer_list<value_type> il) -> FBStringBasic&
{
    append(il);
    return *this;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::append(const FBStringBasic& str) -> FBStringBasic&
{
    append(str.const_raw_data(), str.size());
    return *this;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::append(const FBStringBasic& str, const size_type pos,size_type n) -> FBStringBasic&
{
    const auto left_size = str.size() - pos;
    if (n > left_size)
        n = left_size;
    return append(str.const_raw_data() + pos,n);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::append(const value_type* s, size_type n) -> FBStringBasic&
{
    /* 内部实现支持 2 种类型的 s,s + n;
     * 1. s,s + n;位于当前字符串对象内部;
     * 2. s,s + n;与当前字符串对象毫无交集.
     */

    if (FBSTRING_UNLIKELY(!n))
        return *this;

    auto const old_size = size();
    auto const old_data = const_raw_data();
    value_type *dst = nullptr;

    std::less_equal<const value_type*> le;
    if (FBSTRING_UNLIKELY(le(old_data, s) && !le(old_data + old_size, s))) {
        // 表明 s,s + n 位于当前字符串内部;
        const size_type offset = s - old_data;
        dst = store_.expand_noinit(n,true);
        s = const_raw_data() + offset;
    } else {
        dst = store_.expand_noinit(n,true);
    }

    fbstring_detail::pod_copy(s, s + n, dst);
    return *this;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::append(const value_type* s) -> FBStringBasic&
{
    return append(s, traits_type::length(s));
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::append(size_type n, value_type c) -> FBStringBasic&
{
    resize(size() + n, c);
    return *this;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::append(std::initializer_list<value_type> il) -> FBStringBasic&
{
    return append(il.begin(), il.end());
}

template <typename E, class S, class T, class A> template<class InputIterator>
auto FBStringBasic<E,S,T,A>::append(InputIterator first, InputIterator last) -> FBStringBasic&
{
    insert(end(), first, last);
    return *this;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::assign(const value_type* s, const size_type n) -> FBStringBasic&
{
    /* 内部实现支持 2 种类型的 s,s + n;
     * 1. s,s + n;位于当前字符串对象内部;
     * 2. s,s + n;与当前字符串对象毫无交集.
     */

    const size_type old_size = size();
    if (old_size >= n) {
        fbstring_detail::pod_move(s, s + n, store_.mutable_data());
        resize(n);
    } else {
        const value_type *const s2 = s + old_size;
        /* 这里使用 pod_copy() 即可,因为 size() < n;所以 s,s + n 可以确定了不会与当前字
         * 符串对象重叠.所以不清楚为啥 folly 里面还是用的 pod_move().
         */
        fbstring_detail::pod_copy(s, s2, store_.mutable_data());
        append(s2, n - old_size);
    }
    return *this;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::assign(const FBStringBasic& str) -> FBStringBasic&
{
    if (FBSTRING_UNLIKELY(&str == this))
        return *this;

    return assign(str.const_raw_data(), str.size());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::assign(FBStringBasic&& str) -> FBStringBasic
{
    return *this = std::move(str);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::assign(const FBStringBasic& str, const size_type pos,size_type n) -> FBStringBasic&
{
    const auto left_size = str.size() - pos;
    if (n > left_size)
        n = left_size;
    return assign(str.const_raw_data() + pos,n);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::assign(const value_type* s) -> FBStringBasic&
{
    return assign(s, traits_type::length(s));
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::assign(std::initializer_list<value_type> il) -> FBStringBasic&
{
    return assign(il.begin(), il.end());
}

template <typename E, class S, class T, class A>  template <class ItOrLength, class ItOrChar>
auto FBStringBasic<E,S,T,A>::assign(ItOrLength first_or_n, ItOrChar last_or_c) -> FBStringBasic&
{
    return replace(begin(), end(), first_or_n, last_or_c);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::insert(size_type pos1, const FBStringBasic& str) -> FBStringBasic&
{
    return insert(pos1, str.const_raw_data(), str.size());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::insert(size_type pos1, const FBStringBasic& str,size_type pos2, size_type n) -> FBStringBasic&
{
    const auto left_size = str.size() - pos2;
    if (n > left_size)
        n = left_size;

    return insert(pos1, str.const_raw_data() + pos2, n);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::insert(size_type pos, const value_type* s, size_type n) -> FBStringBasic&
{
    insert(cbegin() + pos, s, s + n);
    return *this;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::insert(size_type pos, const value_type* s) -> FBStringBasic&
{
    return insert(pos, s, traits_type::length(s));
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::insert(size_type pos, size_type n, value_type c) -> FBStringBasic&
{
    insert(cbegin() + pos, n, c);
    return *this;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::insert(const_iterator p, const value_type c) -> iterator
{
    const size_type pos = p - cbegin();
    insert(p, 1, c);
    return begin() + pos;
}

template <typename E, class S, class T, class A> template <class ItOrLength, class ItOrChar>
auto FBStringBasic<E,S,T,A>::insert(const_iterator p, ItOrLength first_or_n, ItOrChar last_or_c) -> iterator
{
    Selector<std::numeric_limits<ItOrLength>::is_specialized> sel;
    return insertImplDiscr(p, first_or_n, last_or_c, sel);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::insert(const_iterator p, std::initializer_list<value_type> il) -> iterator
{
    return insert(p, il.begin(), il.end());
}


template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::insertImplDiscr(const_iterator i,size_type n, value_type c, Selector<1>) -> iterator
{
    const size_type pos = i - cbegin();
    const auto old_size = size();

    store_.expand_noinit(n, /* expGrowth = */ true);
    auto b = begin();
    auto retval = b + pos;

    fbstring_detail::pod_move(retval, b + old_size, retval + n);
    fbstring_detail::PodFill(retval, retval + n, c);

    return retval;
}

template <typename E, class S, class T, class A> template<class InputIter>
auto FBStringBasic<E,S,T,A>::insertImplDiscr(const_iterator i,InputIter b, InputIter e, Selector<0>) -> iterator
{
    return insertImpl(i, b, e,typename std::iterator_traits<InputIter>::iterator_category{});
}

template <typename E, class S, class T, class A> template <class FwdIterator>
auto FBStringBasic<E,S,T,A>::insertImpl(const_iterator i,FwdIterator s1,FwdIterator s2,std::forward_iterator_tag) -> iterator
{
    const size_type pos = i - cbegin();
    const auto n = std::distance(s1, s2);
    const auto old_size = size();

    store_.expand_noinit(n, /* expGrowth = */ true);
    const auto b = begin();
    const auto insert_pointer = b + pos;

    fbstring_detail::pod_move(insert_pointer, b + old_size, insert_pointer + n);
    std::copy(s1, s2, insert_pointer);

    return insert_pointer;
}

template <typename E, class S, class T, class A> template <class InputIterator>
auto FBStringBasic<E,S,T,A>::insertImpl(const_iterator i,InputIterator b, InputIterator e,std::input_iterator_tag) -> iterator
{
    const auto pos = i - cbegin();
    FBStringBasic temp(cbegin(), i);
    for (; b != e; ++b)
        temp.push_back(*b);
    temp.append(i, cend());

    swap(std::move(temp));
    return begin() + pos;
}

template <typename E, class S, class T, class A> template <class ItOrLength, class ItOrChar>
auto FBStringBasic<E,S,T,A>::insert(const_iterator p, ItOrLength first_or_n, ItOrChar last_or_c) -> iterator
{
    Selector<std::numeric_limits<ItOrLength>::is_specialized> sel;
    return insertImplDiscr(p, first_or_n, last_or_c, sel);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::insert(const_iterator p, std::initializer_list<value_type> il) -> iterator
{
    return insert(p, il.begin(), il.end());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::erase(size_type pos = 0, size_type n) -> FBStringBasic&
{
    const auto left_size = size() - pos;
    if (n > left_size)
        n = left_size;

    if (FBSTRING_UNLIKELY(n == 0))
        return *this;

    const auto b = begin();
    const auto e = end();
    const auto erased_pointer = b + pos;
    std::copy(erased_pointer + n, e, erased_pointer);
    resize(size() - n);
    return *this;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::erase(iterator position) -> iterator
{
    const size_type pos {const_iterator{position} - cbegin()};
    erase(pos, 1);
    return begin() + pos;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::erase(iterator first, iterator last) -> iterator
{
    const const_iterator const_first{first};
    const const_iterator const_last{last};

    const size_type pos {const_first - cbegin()};
    erase(pos, const_last - const_first);
    return begin() + pos;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::replace(size_type pos1, size_type n1,const FBStringBasic& str) -> FBStringBasic&
{
    return replace(pos1, n1, str.const_raw_data(), str.size());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::replace(size_type pos1, size_type n1,const FBStringBasic& str,size_type pos2, size_type n2) -> FBStringBasic&
{
    return replace(pos1, n1, str.const_raw_data() + pos2,std::min(n2, str.size() - pos2));
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::replace(size_type pos, size_type n1, const value_type* s) -> FBStringBasic&
{
    return replace(pos, n1, s, traits_type::length(s));
}

template <typename E, class S, class T, class A> template <class StrOrLength, class NumOrChar>
auto FBStringBasic<E,S,T,A>::replace(size_type pos, size_type n1,StrOrLength s_or_n2, NumOrChar n_or_c) -> FBStringBasic&
{
    const auto left_size = size() - pos;
    if (n > left_size)
        n = left_size;

    /* 此处需要考虑 [s,n) 可能是当前字符串的一部分;
     * - 若当前字符串.isShared() 返回 false,则调用 begin() 不会导致 fork 内存,从而 s,n 有效;
     * - 若当前字符串.isShared() 返回 true,则调用 begin() 虽然会导致 fork 内存,但是 s,n 确定
     *   的区间仍然被其他字符串对象引用所以也不会被释放,即保持有效.
     */
    const iterator b = begin() + pos;
    return replace(b, b + n1, s_or_n2, n_or_c);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::replace(iterator i1, iterator i2, const FBStringBasic& str) -> FBStringBasic&
{
    return replace(i1, i2, str.const_raw_data(), str.size());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::replace(iterator i1, iterator i2, const value_type* s) -> FBStringBasic&
{
    return replace(i1, i2, s, traits_type::length(s));
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::replaceImplDiscr(iterator i1, iterator i2,const value_type* s, size_type n,Selector<2>) -> FBStringBasic&
{
    return replace(i1, i2, s, s + n);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::replaceImplDiscr(iterator i1, iterator i2,size_type n2, value_type c, Selector<1>) -> FBStringBasic&
{
    const size_type n1 = i2 - i1;
    if (n1 > n2) {
        std::fill(i1, i1 + n2, c);
        erase(i1 + n2, i2);
    } else {
        std::fill(i1, i2, c);
        insert(i2, n2 - n1, c);
    }
    return *this;
}

template <typename E, class S, class T, class A> template <class InputIter>
auto FBStringBasic<E,S,T,A>::replaceImplDiscr(iterator i1, iterator i2,InputIter b, InputIter e,Selector<0>) -> FBStringBasic&
{
    replaceImpl(i1, i2, b, e,typename std::iterator_traits<InputIter>::iterator_category{});
    return *this;
}

template <typename E, class S, class T, class A> template <class FwdIterator>
bool FBStringBasic<E,S,T,A>::replaceAliased(iterator /* i1 */,iterator /* i2 */,FwdIterator /* s1 */,FwdIterator /* s2 */,std::false_type)
{
    return false;
}

template <typename E, class S, class T, class A> template <class FwdIterator>
bool FBStringBasic<E,S,T,A>::replaceAliased(iterator i1, iterator i2,FwdIterator s1, FwdIterator s2, std::true_type)
{
    /* folly 里面是这样写的:
     *   static const std::less_equal<const value_type*> le = std::less_equal<const value_type*>{};
     *
     * 在 C++11 中,static 是线程安全的,而在 g++ 的实现中,static 采用 pthread_once 来实现,涉及到原子操作以及同步,
     * 所以我觉得这里不如直接使用局部变量了.
     */
    std::less_equal<const value_type*> le{};

    /* aliased 若为真,则表明 [s1,s2) 与当前字符串交集;否则表明无交集.
     * 注意当调用此函数时,s1,s2 的类型要么是 iterator,要么是 const_iterator 类型.
     */
    const bool aliased = le(&*cbegin(), &*s1) && le(&*s1, &*cend());
    if (!aliased)
        return false;

    // Aliased replace, copy to new string
    FBStringBasic temp;
    temp.reserve(size() - (i2 - i1) + std::distance(s1, s2));
    temp.append(cbegin(), const_iterator{i1}).append(s1, s2).append(const_iterator{i2}, cend());
    swap(temp);
    return true;
}

template <typename E, class S, class T, class A> template <class FwdIterator>
void FBStringBasic<E,S,T,A>::replaceImpl(iterator i1, iterator i2,FwdIterator s1, FwdIterator s2, std::forward_iterator_tag)
{
    if (replaceAliased(i1, i2, s1, s2,
          std::integral_constant<bool,
            std::is_same<FwdIterator, iterator>::value ||
            std::is_same<FwdIterator, const_iterator>::value>()))
    {
        return;
    }

    auto const n1 = i2 - i1;
    auto const n2 = std::distance(s1, s2);

    if (n1 > n2) {
        std::copy(s1, s2, i1);
        erase(i1 + n2, i2);
    } else {
        fbstring_detail::CopyN(s1, n1, i1);
        std::advance(s1, n1);
        insert(i2, s1, s2);
    }
    return ;
}

template <typename E, class S, class T, class A> template <class InputIterator>
void FBStringBasic<E,S,T,A>::replaceImpl(iterator i1, iterator i2,InputIterator b, InputIterator e, std::input_iterator_tag)
{
    FBStringBasic temp(cbegin(), const_iterator{i1});
    temp.append(b, e).append(const_iterator{i2}, cend());
    swap(temp);
    return ;
}

template <typename E, class S, class T, class A> template <class T1, class T2>
auto FBStringBasic<E,S,T,A>::replace(iterator i1, iterator i2,T1 first_or_n_or_s, T2 last_or_c_or_n) -> FBStringBasic&
{
    const bool num1 = std::numeric_limits<T1>::is_specialized;
    const bool num2 = std::numeric_limits<T2>::is_specialized;
    return replaceImplDiscr(i1, i2, first_or_n_or_s, last_or_c_or_n,
                Selector<num1 ? (num2 ? 1 : -1) : (num2 ? 2 : 0)>{});
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::copy(value_type* s, size_type n, size_type pos = 0) const -> size_type
{
    const auto left_size = size() - pos;
    if (n > left_size)
        n = left_size;

    const value_type *copy_region_begin = const_raw_data() + pos;
    fbstring_detail::pod_copy(copy_region_begin,copy_region_begin + n,s);
    return n;
}

template <typename E, class S, class T, class A>
void FBStringBasic<E,S,T,A>::swap(FBStringBasic& rhs)
{
    store_.swap(rhs.store_);
    return ;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::c_str() const -> const value_type*
{
    return store_.c_str();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::data() -> value_type*
{
    const auto current_size = store_.size();
    store_.reserve(current_size + 1);
    value_type *ptr = store_.mutable_data();
    ptr[current_size] = '\0';
    return ptr;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::data() const -> const value_type*
{
    return c_str();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::cdata() const -> const value_type*
{
    return c_str();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::raw_data() -> value_type*
{
    return store_.mutable_data();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::raw_data() const -> const value_type*
{
    return store_.data();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::const_raw_data() const -> const value_type*
{
    return raw_data();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::get_allocator() const -> allocator_type
{
    return allocator_type();
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find(const FBStringBasic& str, size_type pos = 0) const -> size_type
{
    return find(str.const_raw_data(), pos, str.length());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find(const value_type* needle, const size_type pos,const size_type nsize) const -> size_type
{
    if (FBSTRING_UNLIKELY(nsize == 0))
        return pos;

    auto const size = this->size();

    {
        const auto tmp = nsize + pos;
        if (tmp > size || tmp < pos)
            return npos;
    }

    // Don't use std::search, use a Boyer-Moore-like trick by comparing
    // the last characters first
    auto const haystack = const_raw_data();
    auto const nsize_1 = nsize - 1;
    auto const last_needle = needle[nsize_1];

    // Boyer-Moore skip value for the last char in the needle. Zero is
    // not a valid value; skip will be computed the first time it's
    // needed.
    size_type skip = 0;

    const E * i = haystack + pos;
    auto i_end = haystack + size - nsize_1;

    while (i < i_end) {
        // Boyer-Moore: match the last element in the needle
        while (i[nsize_1] != last_needle) {
            if (++i == i_end) {
                // not found
                return npos;
            }
        }

        // Here we know that the last char matches
        // Continue in pedestrian mode
        for (size_t j = 0; ; ) {
            if (i[j] != needle[j]) {
                // Not found, we can skip
                // Compute the skip value lazily
                if (skip == 0) {
                    skip = 1;
                    while (skip <= nsize_1 && needle[nsize_1 - skip] != last_needle) {
                        ++skip;
                    }
                }
                i += skip;
                break;
            }
            // Check if done searching
            if (++j == nsize) {
                // Yay
                return i - haystack;
            }
        }
    }
    return npos;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find(const value_type* s, size_type pos = 0) const -> size_type
{
    return find(s, pos, traits_type::length(s));
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find(value_type c, size_type pos = 0) const -> size_type
{
    return find(&c, pos, 1);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::rfind(const FBStringBasic& str, size_type pos = npos) const -> size_type
{
    return rfind(str.const_raw_data(), pos, str.length());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::rfind(const value_type* s, size_type pos, size_type n) const -> size_type
{
    if (n > length())
        return npos;

    pos = std::min(pos, length() - n);
    if (n == 0)
        return pos;

    const_iterator i(cbegin() + pos);
    for (; ; --i) {
        if (traits_type::eq(*i, *s) && traits_type::compare(&*i, s, n) == 0)
            return i - cbegin();

        if (i == cbegin())
            break;
    }
    return npos;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::rfind(const value_type* s, size_type pos = npos) const -> size_type
{
    return rfind(s, pos, traits_type::length(s));
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::rfind(value_type c, size_type pos = npos) const -> size_type
{
    return rfind(&c, pos, 1);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_first_of(const FBStringBasic& str, size_type pos = 0) const -> size_type
{
    return find_first_of(str.const_raw_data(), pos, str.length());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_first_of(const value_type* s,size_type pos, size_type n) const -> size_type
{
    if (pos > length() || n == 0)
        return npos;

    const_iterator i(cbegin() + pos);
    const_iterator finish(cend());
    for (; i != finish; ++i) {
        if (traits_type::find(s, n, *i) != 0)
            return i - cbegin();
    }
    return npos;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_first_of(const value_type* s, size_type pos = 0) const -> size_type
{
    return find_first_of(s, pos, traits_type::length(s));
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_first_of(value_type c, size_type pos = 0) const -> size_type
{
    return find_first_of(&c, pos, 1);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_last_of (const FBStringBasic& str,size_type pos = npos) const -> size_type
{
    return find_last_of(str.const_raw_data(),pos,str.size());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_last_of (const value_type* s, size_type pos,size_type n) const -> size_type
{
    if (!empty() && n > 0) {
        pos = std::min(pos, length() - 1);

        const_iterator i(cbegin() + pos);
        for (;; --i) {
            if (traits_type::find(s, n, *i) != 0)
                return i - cbegin();

            if (i == cbegin())
                break;
        }
    }
    return npos;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_last_of (const value_type* s,size_type pos = npos) const -> size_type
{
    return find_last_of(s, pos, traits_type::length(s));
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_last_of (value_type c, size_type pos = npos) const -> size_type
{
    return find_last_of(&c, pos, 1);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_first_not_of(const FBStringBasic& str,size_type pos = 0) const -> size_type
{
    return find_first_not_of(str.const_raw_data(), pos, str.size());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_first_not_of(const value_type* s, size_type pos,size_type n) const -> size_type
{
    if (pos < length()) {
        const_iterator i(cbegin() + pos), finish(cend());
        for (; i != finish; ++i) {
            if (traits_type::find(s, n, *i) == 0)
                return i - cbegin();
        }
    }
    return npos;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_first_not_of(const value_type* s,size_type pos = 0) const -> size_type
{
    return find_first_not_of(s, pos, traits_type::length(s));
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_first_not_of(value_type c, size_type pos = 0) const -> size_type
{
    return find_first_not_of(&c, pos, 1);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_last_not_of(const FBStringBasic& str,size_type pos = npos) const -> size_type
{
    return find_last_not_of(str.const_raw_data(), pos, str.length());
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_last_not_of(const value_type* s, size_type pos,size_type n) const -> size_type
{
    if (!this->empty()) {
        pos = std::min(pos, size() - 1);

        const_iterator i(cbegin() + pos);
        for (;; --i) {
            if (traits_type::find(s, n, *i) == 0)
                return i - cbegin();
            if (i == cbegin())
                break;
        }
    }
    return npos;
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_last_not_of(const value_type* s,size_type pos = npos) const -> size_type
{
    return find_last_not_of(s, pos, traits_type::length(s));
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::find_last_not_of (value_type c, size_type pos = npos) const -> size_type
{
    return find_last_not_of(&c, pos, 1);
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::substr(size_type pos = 0, size_type n = npos) const & -> FBStringBasic
{
    return FBStringBasic(const_raw_data() + pos, std::min(n, size() - pos));
}

template <typename E, class S, class T, class A>
auto FBStringBasic<E,S,T,A>::substr(size_type pos = 0, size_type n = npos) && -> FBStringBasic
{
    erase(0, pos);
    if (n < size())
        resize(n);
    return std::move(*this);
}

template <typename E, class S, class T, class A>
int FBStringBasic<E,S,T,A>::compare(const FBStringBasic& str) const
{
    return compare(0, size(), str);
}

template <typename E, class S, class T, class A>
int FBStringBasic<E,S,T,A>::compare(size_type pos1, size_type n1,const FBStringBasic& str) const
{
    return compare(pos1, n1, str.const_raw_data(), str.size());
}


template <typename E, class S, class T, class A>
int FBStringBasic<E,S,T,A>::compare(size_type pos1, size_type n1,const value_type* s) const
{
    return compare(pos1, n1, s, traits_type::length(s));
}

template <typename E, class S, class T, class A>
int FBStringBasic<E,S,T,A>::compare(size_type pos1, size_type n1,const value_type* s, size_type n2) const
{
    const auto left_size = size() - pos;
    if (n1 > left_size)
        n1 = left_size;

    const int r = traits_type::compare(pos1 + const_raw_data(), s, std::min(n1, n2));
    return r != 0 ? r : (n1 > n2 ? 1 : (n1 < n2 ? -1 : 0));
}

template <typename E, class S, class T, class A>
int FBStringBasic<E,S,T,A>::compare(size_type pos1, size_type n1,const FBStringBasic& str,size_type pos2, size_type n2) const
{
    return compare(pos1, n1, str.const_raw_data() + pos2,std::min(n2, str.size() - pos2));
}

template <typename E, class S, class T, class A>
int FBStringBasic<E,S,T,A>::compare(const value_type* s) const
{
    // 这里会有一次不必要的 if 判断!
    return compare(0, size(), s, traits_type::length(s));
}

#undef FBSTRING_LIKELY
#undef FBSTRING_UNLIKELY

#endif // ORG_PP_QQ_COMMON_STRING_FB_STRING_BASIC_H

