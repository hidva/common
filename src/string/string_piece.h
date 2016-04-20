#ifndef ORG_PP_QQ_COMMON_STRING_STRING_PIECE_H
#define ORG_PP_QQ_COMMON_STRING_STRING_PIECE_H

#include <string>
#include <stdint.h>

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
    inline StringPiece(StringPiece &&);
    inline StringPiece(void *ptr,size_t size) noexcept;

    inline StringPiece(void *ptr,size_t size,const StringPiece &,size_type pos,size_type n = npos);
    inline StringPiece(void *ptr,size_t size,const value_type* s);
    inline StringPiece(void *ptr,size_t size,const value_type* s, size_type n);
    inline StringPiece(void *ptr,size_t size,size_type n, value_type c);
    inline StringPiece(void *ptr,size_t size,const value_type* b, const value_type* e);

    inline ~StringPiece() = default;

    inline StringPiece& operator=(const StringPiece& lhs);
    inline StringPiece& operator=(StringPiece&& goner) noexcept;
    inline StringPiece& operator=(const value_type* s);
    inline StringPiece& operator=(value_type c);
    inline StringPiece& operator=(std::initializer_list<value_type> il);

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

    inline StringPiece& operator+=(const StringPiece& str);
    inline StringPiece& operator+=(const value_type* s);
    inline StringPiece& operator+=(const value_type c);
    inline StringPiece& operator+=(std::initializer_list<value_type> il);

    inline StringPiece& append(const StringPiece& str);
    inline StringPiece& append(const StringPiece& str, const size_type pos,size_type n);
    StringPiece& append(const value_type* s, size_type n);
    inline StringPiece& append(const value_type* s);
    inline StringPiece& append(size_type n, value_type c);
    inline StringPiece& append(std::initializer_list<value_type> il);

    inline StringPiece& assign(const StringPiece& str);
    inline StringPiece& assign(StringPiece&& str);
    inline StringPiece& assign(const StringPiece& str, const size_type pos,size_type n);
    StringPiece& assign(const value_type* s, const size_type n);
    inline StringPiece& assign(const value_type* s);
    inline StringPiece& assign(std::initializer_list<value_type> il);
    inline StringPiece& assign(size_type n, value_type c);

    inline StringPiece& insert(size_type pos1, const StringPiece& str);
    inline StringPiece& insert(size_type pos1, const StringPiece& str,size_type pos2, size_type n);
    inline StringPiece& insert(size_type pos, const value_type* s, size_type n);
    inline StringPiece& insert(size_type pos, const value_type* s);
    inline StringPiece& insert(size_type pos, size_type n, value_type c);
    inline iterator insert(const_iterator p, const value_type c);
    inline iterator insert(const_iterator p, size_type n, value_type c);
    inline iterator insert(const_iterator p, std::initializer_list<value_type> il);

    inline StringPiece& erase(size_type pos = 0, size_type n = npos);
    inline iterator erase(iterator position);
    inline iterator erase(iterator first, iterator last);

    inline StringPiece& replace(size_type pos1, size_type n1,const StringPiece& str);
    inline StringPiece& replace(size_type pos1, size_type n1,const StringPiece& str,size_type pos2, size_type n2);
    inline StringPiece& replace(size_type pos, size_type n1, const value_type* s);
    inline StringPiece& replace(size_type pos, size_type n1,const value_type* s, size_type n);
    inline StringPiece& replace(size_type pos, size_type n1,size_type n2, value_type c);
    inline StringPiece& replace(iterator i1, iterator i2, const StringPiece& str);
    inline StringPiece& replace(iterator i1, iterator i2, const value_type* s);
    inline StringPiece& replace(iterator i1, iterator i2,const value_type* b, const value_type* e);
    inline StringPiece& replace(iterator i1, iterator i2,size_type n, value_type c);
    inline StringPiece& replace(iterator i1, iterator i2,const value_type* s, size_type n);

    inline size_type copy(value_type* s, size_type n, size_type pos = 0) const;

    inline void swap(StringPiece& rhs);

    inline const value_type* c_str() const;

    inline value_type* data();
    inline const value_type* data() const;
    inline const value_type* cdata() const;

    inline value_type* raw_data();
    inline const value_type* raw_data() const;
    inline const value_type* const_raw_data() const;

    inline allocator_type get_allocator() const;

    inline size_type find(const StringPiece& str, size_type pos = 0) const;
    size_type find(const value_type* needle, const size_type pos,const size_type nsize) const;
    inline size_type find(const value_type* s, size_type pos = 0) const ;
    inline size_type find (value_type c, size_type pos = 0) const;

    inline size_type rfind(const StringPiece& str, size_type pos = npos) const;
    size_type rfind(const value_type* s, size_type pos, size_type n) const;
    inline size_type rfind(const value_type* s, size_type pos = npos) const;
    inline size_type rfind(value_type c, size_type pos = npos) const;

    inline size_type find_first_of(const StringPiece& str, size_type pos = 0) const;
    size_type find_first_of(const value_type* s,size_type pos, size_type n) const;
    inline size_type find_first_of(const value_type* s, size_type pos = 0) const;
    inline size_type find_first_of(value_type c, size_type pos = 0) const;

    inline size_type find_last_of (const StringPiece& str,size_type pos = npos) const;
    size_type find_last_of (const value_type* s, size_type pos,size_type n) const;
    inline size_type find_last_of (const value_type* s,size_type pos = npos) const;
    inline size_type find_last_of (value_type c, size_type pos = npos) const;

    inline size_type find_first_not_of(const StringPiece& str,size_type pos = 0) const;
    size_type find_first_not_of(const value_type* s, size_type pos,size_type n) const;
    inline size_type find_first_not_of(const value_type* s,size_type pos = 0) const;
    inline size_type find_first_not_of(value_type c, size_type pos = 0) const;

    inline size_type find_last_not_of(const StringPiece& str,size_type pos = npos) const;
    size_type find_last_not_of(const value_type* s, size_type pos,size_type n) const;
    inline size_type find_last_not_of(const value_type* s,size_type pos = npos) const;
    inline size_type find_last_not_of (value_type c, size_type pos = npos) const;

    inline StringPiece substr(size_type pos = 0, size_type n = npos) const &;
    inline StringPiece substr(size_type pos = 0, size_type n = npos) &&;

    inline int compare(const StringPiece& str) const;
    inline int compare(size_type pos1, size_type n1,const StringPiece& str) const;
    inline int compare(size_type pos1, size_type n1,const value_type* s) const;
    inline int compare(size_type pos1, size_type n1,const value_type* s, size_type n2) const;
    inline int compare(size_type pos1, size_type n1,const StringPiece& str,size_type pos2, size_type n2) const;
    inline int compare(const value_type* s) const;

private:
    char *buf_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

private:
    StringPiece() = delete;
    StringPiece(const StringPiece &) = delete;
};

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

StringPiece::StringPiece(void *ptr,size_t size,const StringPiece &other,size_type pos,size_type n = npos):
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

}

StringPiece& StringPiece::operator=(value_type c)
{

}

StringPiece& StringPiece::operator=(std::initializer_list<value_type> il)
{

}


#endif // ORG_PP_QQ_COMMON_STRING_STRING_PIECE_H
