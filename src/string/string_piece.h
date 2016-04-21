#ifndef ORG_PP_QQ_COMMON_STRING_STRING_PIECE_H
#define ORG_PP_QQ_COMMON_STRING_STRING_PIECE_H

#include <stdint.h>

#include <string>
#include <experimental/string_view>

#include "exception/errno_exception.h"

/* StringPiece 内部是延迟追加'\0'的.
 */
struct StringPiece {
    typedef std::char_traits<char> traits_type;
    typedef typename traits_type::char_type value_type;

    typedef std::allocator<char> allocator_type;
    typedef typename std::allocator<char>::size_type size_type;
    typedef typename std::allocator<char>::difference_type difference_type;
    typedef typename std::allocator<char>::reference reference;
    typedef typename std::allocator<char>::const_reference const_reference;
    typedef typename std::allocator<char>::pointer pointer;
    typedef typename std::allocator<char>::const_pointer const_pointer;

    typedef char* iterator;
    typedef const char* const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

public:
    static constexpr size_type npos = UINT_FAST32_MAX;

public:
    inline StringPiece(StringPiece &&) noexcept;
    inline StringPiece(void *ptr,size_t size) noexcept;

    inline StringPiece(void *ptr,size_t size,const StringPiece &,size_type pos,size_type n = npos);
    inline StringPiece(void *ptr,size_t size,const value_type* s);
    inline StringPiece(void *ptr,size_t size,const value_type* s, size_type n);
    inline StringPiece(void *ptr,size_t size,size_type n, value_type c);
    inline StringPiece(void *ptr,size_t size,const value_type* b, const value_type* e);

    inline ~StringPiece() noexcept = default;

    inline StringPiece& operator=(const StringPiece& lhs);
    inline StringPiece& operator=(StringPiece&& goner) noexcept;
    inline StringPiece& operator=(const value_type* s);
    inline StringPiece& operator=(value_type c);
    inline StringPiece& operator=(std::initializer_list<value_type> il);

    inline iterator begin() noexcept;
    inline const_iterator begin()  const noexcept;
    inline const_iterator cbegin()  const noexcept;

    inline iterator end() noexcept;
    inline const_iterator end() const noexcept;
    inline const_iterator cend() const noexcept;

    inline reverse_iterator rbegin() noexcept;
    inline const_reverse_iterator rbegin() const noexcept;
    inline const_reverse_iterator crbegin() const noexcept;

    inline reverse_iterator rend() noexcept;
    inline const_reverse_iterator rend() const noexcept;
    inline const_reverse_iterator crend() const noexcept;

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

    inline void resize(const size_type n, const value_type c = value_type());
    inline void reserve(size_type res_arg = 0);
    inline void shrink_to_fit();
    inline void clear();

    inline bool empty() const;

    inline reference operator[](size_type pos);
    inline const_reference operator[](size_type pos) const;

    inline reference at(size_type n);
    inline const_reference at(size_type n) const;
    inline const_reference cat(size_type n) const;

    inline StringPiece& operator+=(const StringPiece& str);
    inline StringPiece& operator+=(const value_type* s);
    inline StringPiece& operator+=(const value_type c);
    inline StringPiece& operator+=(std::initializer_list<value_type> il);

    inline StringPiece& append(const StringPiece& str);
    inline StringPiece& append(const StringPiece& str, const size_type pos,size_type n);
    inline StringPiece& append(const value_type* s, size_type n);
    inline StringPiece& append(const value_type* s);
    inline StringPiece& append(size_type n, value_type c);
    inline StringPiece& append(std::initializer_list<value_type> il);

    template <typename Iter>
    inline StringPiece& append(Iter b,Iter e);

    // assign();
    inline StringPiece& assign(const StringPiece& str);
    inline StringPiece& assign(StringPiece&& str);
    inline StringPiece& assign(const StringPiece& str, const size_type pos,size_type n);
    inline StringPiece& assign(const value_type* s, const size_type n);
    inline StringPiece& assign(const value_type* s);
    inline StringPiece& assign(std::initializer_list<value_type> il);
    inline StringPiece& assign(size_type n, value_type c);

    template< class InputIt >
    inline StringPiece& assign( InputIt first, InputIt last );

    // insert();
    inline StringPiece& insert(size_type pos1, const StringPiece& str);
    inline StringPiece& insert(size_type pos1, const StringPiece& str,size_type pos2, size_type n);
    StringPiece& insert(size_type pos, const value_type* s, size_type n);
    inline StringPiece& insert(size_type pos, const value_type* s);
    inline StringPiece& insert(size_type pos, size_type n, value_type c);
    inline iterator insert(const_iterator p, const value_type c);
    iterator insert(const_iterator p, size_type n, value_type c);
    inline iterator insert(const_iterator p, std::initializer_list<value_type> il);

    template<class InputIt>
    inline iterator insert(const_iterator pos, InputIt first, InputIt last);

    // erase();
    inline StringPiece& erase(size_type pos = 0, size_type n = npos) noexcept;
    inline iterator erase(const_iterator position) noexcept;
    inline iterator erase(const_iterator first, const_iterator last) noexcept;

    // replace
    inline StringPiece& replace(size_type pos1, size_type n1,const StringPiece& str);
    inline StringPiece& replace(size_type pos1, size_type n1,const StringPiece& str,size_type pos2, size_type n2);
    inline StringPiece& replace(size_type pos, size_type n1, const value_type* s);
    inline StringPiece& replace(size_type pos, size_type n1,const value_type* s, size_type n);
    inline StringPiece& replace(size_type pos, size_type n1,size_type n2, value_type c);
    inline StringPiece& replace(const_iterator i1, const_iterator i2, const StringPiece& str);
    inline StringPiece& replace(const_iterator i1, const_iterator i2, const value_type* s);
    StringPiece& replace(const_iterator i1, const_iterator i2,size_type n, value_type c);
    StringPiece& replace(const_iterator i1, const_iterator i2,const value_type* s, size_type n);
    inline StringPiece& replace( const_iterator first, const_iterator last,std::initializer_list<CharT> ilist);

    template< class InputIt >
    inline StringPiece& replace( const_iterator first, const_iterator last,InputIt first2, InputIt last2 );

    inline size_type copy(value_type* s, size_type n, size_type pos = 0) const noexcept;

    inline void swap(StringPiece& rhs);

    inline const value_type* c_str() const;

    inline value_type* data();
    inline const value_type* data() const;
    inline const value_type* cdata() const;

    inline value_type* raw_data() noexcept;
    inline const value_type* raw_data() const noexcept;
    inline const value_type* const_raw_data() const noexcept;

    inline allocator_type get_allocator() const noexcept;

    inline size_type find(const StringPiece& str, size_type pos = 0) const noexcept;
    inline size_type find(const value_type* needle, const size_type pos,const size_type nsize) const noexcept;
    inline size_type find(const value_type* s, size_type pos = 0) const noexcept;
    inline size_type find (value_type c, size_type pos = 0) const noexcept;

    inline size_type rfind(const StringPiece& str, size_type pos = npos) const noexcept;
    inline size_type rfind(const value_type* s, size_type pos, size_type n) const noexcept;
    inline size_type rfind(const value_type* s, size_type pos = npos) const noexcept;
    inline size_type rfind(value_type c, size_type pos = npos) const noexcept;

    inline size_type find_first_of(const StringPiece& str, size_type pos = 0) const noexcept;
    inline size_type find_first_of(const value_type* s,size_type pos, size_type n) const noexcept;
    inline size_type find_first_of(const value_type* s, size_type pos = 0) const noexcept;
    inline size_type find_first_of(value_type c, size_type pos = 0) const noexcept;

    inline size_type find_last_of (const StringPiece& str,size_type pos = npos) const noexcept;
    inline size_type find_last_of (const value_type* s, size_type pos,size_type n) const noexcept;
    inline size_type find_last_of (const value_type* s,size_type pos = npos) const noexcept;
    inline size_type find_last_of (value_type c, size_type pos = npos) const noexcept;

    inline size_type find_first_not_of(const StringPiece& str,size_type pos = 0) const noexcept;
    inline size_type find_first_not_of(const value_type* s, size_type pos,size_type n) const noexcept;
    inline size_type find_first_not_of(const value_type* s,size_type pos = 0) const noexcept;
    inline size_type find_first_not_of(value_type c, size_type pos = 0) const noexcept;

    inline size_type find_last_not_of(const StringPiece& str,size_type pos = npos) const noexcept;
    inline size_type find_last_not_of(const value_type* s, size_type pos,size_type n) const noexcept;
    inline size_type find_last_not_of(const value_type* s,size_type pos = npos) const noexcept;
    inline size_type find_last_not_of (value_type c, size_type pos = npos) const noexcept;

    /* 受限于 StringPiece 的特性,很显然是无法实现出这2个接口的.
     */
#if 0
    inline StringPiece substr(size_type pos = 0, size_type n = npos) const &;
    inline StringPiece substr(size_type pos = 0, size_type n = npos) &&;
#endif

    inline int compare(const StringPiece& str) const noexcept;
    inline int compare(size_type pos1, size_type n1,const StringPiece& str) const noexcept;
    inline int compare(size_type pos1, size_type n1,const value_type* s) const noexcept;
    inline int compare(size_type pos1, size_type n1,const value_type* s, size_type n2) const noexcept;
    inline int compare(size_type pos1, size_type n1,const StringPiece& str,size_type pos2, size_type n2) const noexcept;
    inline int compare(const value_type* s) const noexcept;

private:
    /* 若 n > max,则 n = max;否则无操作
     */
    static inline void AdjustTo(size_type &n,size_type max) noexcept;

    /* 测试当前 RawByteArray 是否'\0'结尾.即 size_ < capacity_ && [size_] = '\0'.
     */
    inline bool IsNullTerminated() const noexcept;

    /* 在可以的情况(size_ < capacity_)下,将其设置为'\0'结尾.否则异常.
     */
    inline void SetNullTerminated() const;

private:
    char * const buf_ = nullptr;
    size_type size_ = 0;
    size_type const capacity_ = 0;

private:
    StringPiece() = delete;
    StringPiece(const StringPiece &) = delete;
};

void StringPiece::AdjustTo(size_type &n,size_type max) noexcept
{
    if (n > max)
        n = max;
    return ;
}

StringPiece::StringPiece(StringPiece &&other):
    buf_(other.buf_),size_(other.size_),capacity_(other.capacity_)
{
    other.buf_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

StringPiece::StringPiece(void *ptr,size_t size) noexcept:
    buf_(ptr),size_(0),capacity_(size)
{
}

StringPiece::StringPiece(void *ptr,size_t size,const StringPiece &other,size_type pos,size_type n):
    StringPiece(ptr,size)
{
    assign(other,pos,n);
}

StringPiece::StringPiece(void *ptr,size_t size,const value_type* s):
    StringPiece(ptr,size,s,strlen(s))
{
}

StringPiece::StringPiece(void *ptr,size_t size,const value_type* s, size_type n):
    StringPiece(ptr,size)
{
    assign(s,n);
}

StringPiece::StringPiece(void *ptr,size_t size,size_type n, value_type c):
    StringPiece(ptr,size)
{
    assign(n,c);
}

StringPiece::StringPiece(void *ptr,size_t size,const value_type* b, const value_type* e):
    StringPiece(ptr,size)
{
    assign(b,e - b);
}

StringPiece& StringPiece::operator=(const StringPiece& lhs)
{
    return assign(lhs);
}

StringPiece& StringPiece::operator=(StringPiece&& goner) noexcept
{
    return assign(std::move(goner));
}

StringPiece& StringPiece::operator=(const value_type* s)
{
    return assign(s);
}

StringPiece& StringPiece::operator=(value_type c)
{
    return assign(1,c);
}

StringPiece& StringPiece::operator=(std::initializer_list<value_type> il)
{
    return assign(il);
}

auto StringPiece::begin() -> iterator
{
    return buf_;
}

auto StringPiece::begin() const -> const_iterator
{
    return buf_;
}

auto StringPiece::cbegin() const -> const_iterator
{
    return begin();
}

auto StringPiece::end() -> iterator
{
    return buf_ + size_;
}

auto StringPiece::end() const -> const_iterator
{
    return buf_ + size_;
}

auto StringPiece::cend() const -> const_iterator
{
    return end();
}


auto StringPiece::rbegin() -> reverse_iterator
{
    return reverse_iterator(end());
}

auto StringPiece::rbegin() const -> const_reverse_iterator
{
    return const_reverse_iterator(end());
}

auto StringPiece::crbegin() const -> const_reverse_iterator
{
    return rbegin();
}

auto StringPiece::rend() -> reverse_iterator
{
    return reverse_iterator(begin());
}

auto StringPiece::rend() const -> const_reverse_iterator
{
    return const_reverse_iterator(begin());
}

auto StringPiece::crend() const -> const_reverse_iterator
{
    return rend();
}

auto StringPiece::front() -> value_type&
{
    return *begin();
}


auto StringPiece::front() const -> const value_type&
{
    return *begin();
}

auto StringPiece::cfront() const -> const value_type&
{
    return front();
}

auto StringPiece::back() -> value_type&
{
    return *(end() - 1);
}

auto StringPiece::back() const -> const value_type&
{
    return *(end() - 1);
}

auto StringPiece::cback() const -> const value_type&
{
    return back();
}

void StringPiece::pop_back()
{
    --size_;
    return ;
}

void StringPiece::push_back(const value_type c)
{
    PP_QQ_CHECK(size_ < capacity_,ENOSPC,"size_: %zu;capacity_: %zu",size_,capacity_);

    buf_[size_] = c;
    ++size_;
    return ;
}


auto StringPiece::size() const -> size_type
{
    return size_;
}

auto StringPiece::length() const -> size_type
{
    return size();
}

auto StringPiece::max_size() const -> size_type
{
    return UINT_FAST32_MAX;
}

auto StringPiece::capacity() const -> size_type
{
    return capacity_;
}

void StringPiece::resize(const size_type n, const value_type c)
{
    PP_QQ_CHECK(n <= capacity_,ENOSPC,"n: %zu;capacity: %zu",n,capacity_);

    if (n > size_)
        memset(buf_ + size_,c,n - size_);
    size_ = n;
    return ;
}

void StringPiece::reserve(size_type res_arg)
{
    PP_QQ_CHECK(res_arg <= capacity_,ENOSPC,"res_arg: %zu;capacity: %zu",res_arg,capacity_);
    return ;
}

void StringPiece::shrink_to_fit()
{
    return ;
}

void StringPiece::clear()
{
    size_ = 0;
    return ;
}

bool StringPiece::empty() const
{
    return size_ == 0;
}

auto StringPiece::operator[](size_type pos) -> reference
{
    return buf_[pos];
}

auto StringPiece::operator[](size_type pos) const -> const_reference
{
    return buf_[pos];
}

auto StringPiece::at(size_type n) -> reference
{
    return buf_[n];
}

auto StringPiece::at(size_type n) const -> const_reference
{
    return buf_[n];
}

auto StringPiece::cat(size_type n) const -> const_reference
{
    return at(n);
}

StringPiece& StringPiece::operator+=(const StringPiece& str)
{
    return append(str);
}

StringPiece& StringPiece::operator+=(const value_type* s)
{
    return append(s);
}

StringPiece& StringPiece::operator+=(const value_type c)
{
    return append(1,c);
}

StringPiece& StringPiece::operator+=(std::initializer_list<value_type> il)
{
    return append(il);
}

StringPiece& StringPiece::append(const StringPiece& str)
{
    return append(str.const_raw_data(),str.size());
}

StringPiece& StringPiece::append(const StringPiece& str, const size_type pos,size_type n)
{
    AdjustTo(n,str.size() - pos);
    return append(str.const_raw_data() + pos,n);
}

StringPiece& StringPiece::append(const value_type* s, size_type n)
{
    size_type new_size = size_ + n;
    PP_QQ_CHECK(new_size <= capacity_,EINVAL,"new_size: %zu;capacity_: %zu",new_size,capacity_);

    memcpy(buf_ + size_,s,n);
    size_ = new_size;
    return *this;
}

StringPiece& StringPiece::append(const value_type* s)
{
    return append(s,strlen(s));
}

StringPiece& StringPiece::append(size_type n, value_type c)
{
    resize(size() + n, c);
    return *this;
}

StringPiece& StringPiece::append(std::initializer_list<value_type> il)
{
    return append(il.begin(),il.end());
}

template <typename Iter>
StringPiece& StringPiece::append(Iter b,Iter e)
{
    static_assert(0,"由于现在对 STL 中整个迭代器模块不是很熟悉,所以还是不要贸然实现这个了吧.");
    return *this;
}

StringPiece& StringPiece::assign(const StringPiece& str)
{
    return assign(str.const_raw_data(),str.size());
}

StringPiece& StringPiece::assign(StringPiece&& str)
{
    return assign(str);
}

StringPiece& StringPiece::assign(const StringPiece& str, const size_type pos,size_type n)
{
    AdjustTo(n,str.size() - pos);
    return assign(str.const_raw_data() + pos,n);
}

StringPiece& StringPiece::assign(const value_type* s, const size_type n)
{
    memmove(buf_,s,n);
    size_ = n;
    return *this;
}

StringPiece& StringPiece::assign(const value_type* s)
{
    return assign(s,strlen(s));
}

StringPiece& StringPiece::assign(std::initializer_list<value_type> il)
{
    return assign(il.begin(),il.end());
}

StringPiece& StringPiece::assign(size_type n, value_type c)
{
    PP_QQ_CHECK(n <= capacity_,EINVAL,"n: %zu;capacity_: %zu",n,capacity_);
    size_ = n;
    memset(buf_,c,size_);
    return *this;
}

template< class InputIt >
StringPiece& StringPiece::assign(InputIt , InputIt )
{
    static_assert(0,"由于现在对 STL 中整个迭代器模块不是很熟悉,所以还是不要贸然实现这个了吧.");
    return *this;
}

StringPiece& StringPiece::insert(size_type pos1, const StringPiece& str)
{
    return insert(pos1,str.const_raw_data(),str.size());
}

StringPiece& StringPiece::insert(size_type pos1, const StringPiece& str,size_type pos2, size_type n)
{
    AdjustTo(n,str.size() - pos2);
    return insert(pos1,str.const_raw_data() + pos2,n);
}

StringPiece& StringPiece::insert(size_type pos, const value_type* s)
{
    return insert(pos,s,strlen(s));
}

auto StringPiece::insert(const_iterator p, const value_type c) -> iterator
{
    const size_type pos = p - cbegin();
    insert(pos,1,c);
    return begin() + pos;
}

auto StringPiece::insert(const_iterator p, size_type n, value_type c) -> iterator
{
    const size_type pos = p - cbegin();
    insert(pos,n,c);
    return begin() + pos;
}

auto StringPiece::insert(const_iterator p, std::initializer_list<value_type> il) -> iterator
{
    return insert(p,il.begin(),il.end());
}

template<class InputIt>
auto StringPiece::insert(const_iterator , InputIt , InputIt ) -> iterator
{
    static_assert(0,"由于现在对 STL 中整个迭代器模块不是很熟悉,所以还是不要贸然实现这个了吧.");
    return *this;
}

auto StringPiece::erase(const_iterator position) noexcept -> iterator
{
    const size_type pos {position - cbegin()};
    erase(pos, 1);
    return begin() + pos;
}


/* TODO(wangwei)
 * 1. 检查所有接口声明,确保与 std::string 一致;再根据实现适当增加 inline,noexcept 标记.
 * 2. 检查所有可能会抛出异常的实现,确保不会因为异常而破坏对象的状态.
 * 3. findX() 系列函数内部使用 string_view,这个得 C++14 才能编译使用.想办法减少这种依赖.!
 */

auto StringPiece::erase(const_iterator first, const_iterator last) noexcept -> iterator
{
    const size_type pos {first - cbegin()};
    erase(pos, last - first);
    return begin() + pos;
}

StringPiece& StringPiece::replace(size_type pos1, size_type n1,const StringPiece& str)
{
    return replace(pos1,n1,str.const_raw_data(),str.size());
}

StringPiece& StringPiece::replace(size_type pos1, size_type n1,const StringPiece& str,size_type pos2, size_type n2)
{
    AdjustTo(n2,str.size() - pos2);
    return replace(pos1,n1,str.const_raw_data() + pos2,n2);
}

StringPiece& StringPiece::replace(size_type pos, size_type n1, const value_type* s)
{
    return replace(pos,n1,s,strlen(s));
}

StringPiece& StringPiece::replace(size_type pos, size_type n1,const value_type* s, size_type n2)
{
    AdjustTo(n1,size() - pos);
    return replace(cbegin() + pos,cbegin() + pos + n1,s,n2);
}

StringPiece& StringPiece::replace(size_type pos, size_type n1,size_type n2, value_type c)
{
    AdjustTo(n1,size() - pos);
    return replace(cbegin() + pos,cbegin() + pos + n1,n2,c);
}

StringPiece& StringPiece::replace(const_iterator i1, const_iterator i2, const StringPiece& str)
{
    return replace(i1,i2,str.const_raw_data(),str.size());
}

StringPiece& StringPiece::replace(const_iterator i1, const_iterator i2, const value_type* s)
{
    return replace(i1,i2,s,strlen(s));
}

StringPiece& StringPiece::replace( const_iterator first, const_iterator last,std::initializer_list<CharT> ilist)
{
    return replace(first,last,ilist.begin(),ilist.end());
}

template<class InputIt>
StringPiece& StringPiece::replace(const_iterator /* first */, const_iterator /* last */,InputIt /* first2 */, InputIt /* last2 */)
{
    static_assert(0,"由于现在对 STL 中整个迭代器模块不是很熟悉,所以还是不要贸然实现这个了吧.");
    return *this;
}

auto StringPiece::copy(value_type* s, size_type n, size_type pos = 0) noexcept -> size_type
{
    AdjustTo(n,size() - pos);
    memcpy(s,const_raw_data() + pos,n);
    return n;
}

void StringPiece::swap(StringPiece& rhs)
{
    PP_QQ_CHECK(rhs.size_ <= capacity_ && size_ <= rhs.capacity_,EINVAL,"size: %zu;capacity: %zu;rhs.size: %zu;rhs.capacity: %zu",size_,capacity_,rhs.size_,rhs.capacity_);

    std::string saver(const_raw_data(),size());
    assign(rhs.const_raw_data(),rhs.size());
    rhs.assign(saver.data(),saver.size());
    return ;
}

bool StringPiece::IsNullTerminated() const noexcept
{
    return size_ < capacity_ && buf_[size_] == '\0';
}

void StringPiece::SetNullTerminated() const
{
    PP_QQ_CHECK(size_ < capacity_,EINVAL,"size: %zu;capacity: %zu",size_,capacity_);
    buf_[size_] = '\0';
    return ;
}

auto StringPiece::c_str() const -> const value_type*
{
    return data();
}

auto StringPiece::data() -> value_type*
{
    SetNullTerminated();
    return buf_;
}

auto StringPiece::data() const -> const value_type*
{
    SetNullTerminated();
    return buf_;
}

auto StringPiece::cdata() const -> const value_type*
{
    return data();
}

auto StringPiece::raw_data() noexcept -> value_type*
{
    return buf_;
}

auto StringPiece::raw_data() const noexcept -> const value_type*
{
    return buf_;
}

auto StringPiece::const_raw_data() const noexcept -> const value_type*
{
    return raw_data();
}

auto StringPiece::get_allocator() const noexcept -> allocator_type
{
    return allocator_type{};
}


auto StringPiece::find(const StringPiece& str, size_type pos) const noexcept -> size_type
{
    return find(str.const_raw_data(), pos, str.length());
}

auto StringPiece::find(const value_type* needle, const size_type pos,const size_type nsize) const noexcept -> size_type
{
    std::experimental::string_view needle_string_view(needle,nsize);
    std::experimental::string_view hack_string_view(const_raw_data(),size());
    const auto find_rc = hack_string_view.find(needle_string_view,pos);
    return find_rc == hack_string_view.npos ? npos : find_rc;
}

auto StringPiece::find(const value_type* s, size_type pos) const noexcept -> size_type
{
    return find(s, pos, strlen(s));
}

auto StringPiece::find (value_type c, size_type pos) const noexcept -> size_type
{
    return find(&c, pos, 1);
}

auto StringPiece::rfind(const StringPiece& str, size_type pos = npos) const noexcept -> size_type
{
    return rfind(str.const_raw_data(),pos,str.size());
}

auto StringPiece::rfind(const value_type* s, size_type pos, size_type n) const noexcept -> size_type
{
    std::experimental::string_view needle_string_view(s,n);
    std::experimental::string_view hack_string_view(const_raw_data(),size());
    const auto find_rc = hack_string_view.rfind(needle_string_view,pos);
    return find_rc == hack_string_view.npos ? npos : find_rc;
}

auto StringPiece::rfind(const value_type* s, size_type pos = npos) const noexcept -> size_type
{
    return rfind(s,pos,strlen(s));
}

auto StringPiece::rfind(value_type c, size_type pos = npos) const noexcept -> size_type
{
    return rfind(&c,pos,1);
}




auto StringPiece::find_first_of(const StringPiece& str, size_type pos) const noexcept -> size_type
{
    return find_first_of(str.const_raw_data(),pos,str.size());
}

auto StringPiece::find_first_of(const value_type* s,size_type pos, size_type n) const noexcept -> size_type
{
    std::experimental::string_view hack_string_view(const_raw_data(),size());
    const auto rc = hack_string_view.find_first_of(s,pos,n);
    return rc == hack_string_view.npos ? npos : rc;
}


auto StringPiece::find_first_of(const value_type* s, size_type pos) const noexcept -> size_type
{
    return find_first_of(s,pos,strlen(s));
}

auto StringPiece::find_first_of(value_type c, size_type pos) const noexcept -> size_type
{
    return find_first_of(&c,pos,1);
}

auto StringPiece::find_last_of (const StringPiece& str,size_type pos) const noexcept -> size_type
{
    return find_last_of(str.const_raw_data(),pos,str.size());
}

auto StringPiece::find_last_of (const value_type* s, size_type pos,size_type n) const noexcept -> size_type
{
    std::experimental::string_view hack_string_view(const_raw_data(),size());
    const auto rc = hack_string_view.find_last_of(s,pos,n);
    return rc == hack_string_view.npos ? npos : rc;
}

auto StringPiece::find_last_of (const value_type* s,size_type pos) const noexcept -> size_type
{
    return find_last_of(s,pos,strlen(s));
}

auto StringPiece::find_last_of (value_type c, size_type pos) const noexcept -> size_type
{
    return find_last_of(&c,pos,1);
}

auto StringPiece::find_first_not_of(const StringPiece& str,size_type pos) const noexcept -> size_type
{
    return find_first_not_of(str.const_raw_data(),pos,str.size());
}

auto StringPiece::find_first_not_of(const value_type* s, size_type pos,size_type n) const noexcept -> size_type
{
    std::experimental::string_view hack_string_view(const_raw_data(),size());
    const auto rc = hack_string_view.find_first_not_of(s,pos,n);
    return rc == hack_string_view.npos ? npos : rc;
}

auto StringPiece::find_first_not_of(const value_type* s,size_type pos) const noexcept -> size_type
{
    return find_first_not_of(s,pos,strlen(s));
}

auto StringPiece::find_first_not_of(value_type c, size_type pos) const noexcept -> size_type
{
    return find_first_not_of(&c,pos,1);
}

auto StringPiece::find_last_not_of(const StringPiece& str,size_type pos) const noexcept -> size_type
{
    return find_last_not_of(str.const_raw_data(),pos,str.size());
}

auto StringPiece::find_last_not_of(const value_type* s, size_type pos,size_type n) const noexcept -> size_type
{
    std::experimental::string_view hack_string_view(const_raw_data(),size());
    const auto rc = hack_string_view.find_last_not_of(s,pos,n);
    return rc == hack_string_view.npos ? npos : rc;
}

auto StringPiece::find_last_not_of(const value_type* s,size_type pos) const noexcept -> size_type
{
    return find_last_not_of(s,pos,strlen(s));
}

auto StringPiece::find_last_not_of (value_type c, size_type pos) const noexcept -> size_type
{
    return find_last_not_of(&c,pos,1);
}

int StringPiece::compare(const StringPiece& str) const noexcept
{
    return compare(0,size(),str);
}

int StringPiece::compare(size_type pos1, size_type n1,const StringPiece& str) const noexcept
{
    return compare(pos1,n1,str.const_raw_data(),str.size());
}

int StringPiece::compare(size_type pos1, size_type n1,const value_type* s) const noexcept
{
    return compare(pos1,n1,s,strlen(s));
}

int StringPiece::compare(size_type pos1, size_type n1,const value_type* s2, size_type n2) const noexcept
{
    AdjustTo(n1,size_ - pos1);

    const int r = memcmp(const_raw_data() + pos1, s2, std::min(n1, n2));
    return r != 0 ? r : (n1 > n2 ? 1 : (n1 < n2 ? -1 : 0));
}

int StringPiece::compare(size_type pos1, size_type n1,const StringPiece& str,size_type pos2, size_type n2) const noexcept
{
    AdjustTo(n2,str.size() - pos2);
    return compare(pos1,n1,str.const_raw_data() + pos2,n2);
}

int StringPiece::compare(const value_type* s) const noexcept
{
    return compare(0,size(),s);
}




























#endif // ORG_PP_QQ_COMMON_STRING_STRING_PIECE_H
