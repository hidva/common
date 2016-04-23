#include "string_piece.h"

StringPiece& StringPiece::insert(size_type pos, size_type n, value_type c)
{
    const auto new_size = size_ + n;
    PP_QQ_CHECK(new_size <= capacity_,EINVAL,"new_size: %zu;capacity_: %zu",new_size,capacity_);

    value_type * const insert_pointer = buf_ + pos;
    memmove(insert_pointer + n,insert_pointer,size_ - pos);
    memset(insert_pointer,c,n);
    size_ = new_size;
    return *this;
}


StringPiece& StringPiece::insert(size_type pos, const value_type* s, size_type n)
{
    const auto new_size = size_ + n;
    PP_QQ_CHECK(new_size <= capacity_,EINVAL,"new_size: %zu;capacity_: %zu",new_size,capacity_);

    if (s >= buf_ && s < buf_ + size_) {
        // [s,s + n) 在当前字符串内部;
        std::string saver(s,n);

        value_type * const insert_pointer = buf_ + pos;
        memmove(insert_pointer + n,insert_pointer,size_ - pos);
        memcpy(insert_pointer,saver.data(),saver.size());
    } else {
        // [s,s + n) 与当前字符串并无任何交集
        value_type * const insert_pointer = buf_ + pos;
        memmove(insert_pointer + n,insert_pointer,size_ - pos);
        memcpy(insert_pointer,s,n);
    }
    size_ = new_size;
    return *this;
}


StringPiece& StringPiece::erase(size_type pos, size_type n) noexcept
{
    const auto left_size = size() - pos;
    if (n >= left_size) {
        size_ = pos;
    } else {
        value_type * const erased_pointer = buf_ + pos;
        memmove(erased_pointer,erased_pointer + n,size_ - (pos + n));
        size_ -= n;
    }
    return *this;
}


StringPiece& StringPiece::replace(const_iterator i1, const_iterator i2,size_type n2, value_type c)
{
    const size_type n1 = i2 - i1;

    const auto new_size = size() - n1 + n2;
    PP_QQ_CHECK(new_size <= capacity_,EINVAL,"new_size: %zu;capacity_: %zu",new_size,capacity_);

    char *replace_pointer = raw_data() + (i1 - cbegin());
    if (n1 > n2) {
        memset(replace_pointer,c,n2);
        erase(i1 + n2, i2);
    } else {
        memset(replace_pointer,c,n1);
        insert(i2, n2 - n1, c);
    }
    return *this;
}

StringPiece& StringPiece::replace(const_iterator i1, const_iterator i2,const value_type* s, size_type n2)
{
    size_type const n1 = i2 - i1;
    const auto new_size = size() - n1 + n2;
    PP_QQ_CHECK(new_size <= capacity_,EINVAL,"new_size: %zu;capacity_: %zu",new_size,capacity_);

    if (s >= buf_ && s < buf_ + size_) {
        // 此时 [s,s + n) 与当前字符串对象重叠.
        std::string saver(s,n2);
        return replace(i1,i2,saver.data(),saver.size());
    }

    char *replace_pointer = raw_data() + (i1 - cbegin());
    if (n1 > n2) {
        memcpy(replace_pointer,s,n2);
        erase(i1 + n2, i2);
    } else {
        memcpy(replace_pointer,s,n1);
        insert(i2 - cbegin(), s + n1, n2 - n1);
    }
    return *this;
}


StringPiece& StringPiece::append(const value_type* s, size_type n)
{
    size_type new_size = size_ + n;
    PP_QQ_CHECK(new_size <= capacity_,EINVAL,"new_size: %zu;capacity_: %zu",new_size,capacity_);

    memcpy(buf_ + size_,s,n);
    size_ = new_size;
    return *this;
}

auto StringPiece::find(const value_type* needle, const size_type pos,const size_type nsize) const noexcept -> size_type
{
    if (pos > size_ || pos < 0)
        return npos;

    if (nsize == 0)
        return pos;

    const value_type *haystack = buf_ + pos;
    const auto haylen = size_ - pos;
    const auto * const rc = memmem(haystack,haylen,needle,nsize);
    return rc == nullptr ? npos : static_cast<const value_type*>(rc) - buf_;
}

auto StringPiece::rfind(const value_type* s, size_type pos, size_type n) const noexcept -> size_type
{
    if (n <= size_) {
        pos = std::min(size_ - n,pos);
        do {
            if (memcmp(buf_ + pos,s,n) == 0)
                return pos;
        } while (pos-- > 0);
    }
    return npos;
}

auto StringPiece::find_first_of(const value_type* s,size_type pos, size_type n) const noexcept -> size_type
{
    if (n == 0)
        return npos;

    for (; pos < size_; ++pos) {
        if (traits_type::find(s,n,buf_[pos]) != nullptr)
            return pos;
    }
    return npos;
}

auto StringPiece::find_last_of (const value_type* s, size_type pos,size_type n) const noexcept -> size_type
{
    size_type current_index = this->size();
    if (current_index && n) {
        if (--current_index > pos)
            current_index = pos;

        do {
            if (traits_type::find(s,n,buf_[current_index]))
                return current_index;
        } while (current_index-- != 0);
    }
    return npos;
}

auto StringPiece::find_first_not_of(const value_type* s, size_type pos,size_type n) const noexcept -> size_type
{
    for (; pos < size_; ++pos) {
        if (!traits_type::find(s,n,buf_[pos]))
            return pos;
    }
    return npos;
}

auto StringPiece::find_last_not_of(const value_type* s, size_type pos,size_type n) const noexcept -> size_type
{
    size_type idx = size_;
    if (idx) {
        if (--idx > pos)
            idx = pos;

        do {
            if (!traits_type::find(s,n,buf_[idx]))
                return idx;
        } while(idx--);
    }
    return npos;
}


void StringPiece::resize(const size_type n, const value_type c)
{
    PP_QQ_CHECK(n <= capacity_,ENOSPC,"n: %zu;capacity: %zu",n,capacity_);

    if (n > size_)
        memset(buf_ + size_,c,n - size_);
    size_ = n;
    return ;
}

void StringPiece::swap(StringPiece& rhs)
{
    PP_QQ_CHECK(rhs.size_ <= capacity_ && size_ <= rhs.capacity_,EINVAL,"size: %zu;capacity: %zu;rhs.size: %zu;rhs.capacity: %zu",size_,capacity_,rhs.size_,rhs.capacity_);

    std::string saver(const_raw_data(),size());
    assign(rhs.const_raw_data(),rhs.size());
    rhs.assign(saver.data(),saver.size());
    return ;
}
