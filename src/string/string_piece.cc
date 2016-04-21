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


StringPiece& StringPiece::erase(size_type pos = 0, size_type n) noexcept
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

    if (n1 > n2) {
        memset(i1,c,n2);
        erase(i1 + n2, i2);
    } else {
        memset(i1,c,n1);
        insert(i2, n2 - n1, c);
    }
    return *this;
}

StringPiece& StringPiece::replace(const_iterator i1, const_iterator i2,const value_type* s, size_type n2)
{
    auto const n1 = i2 - i1;
    const auto new_size = size() - n1 + n2;
    PP_QQ_CHECK(new_size <= capacity_,EINVAL,"new_size: %zu;capacity_: %zu",new_size,capacity_);

    if (s >= buf_ && s < buf_ + size_) {
        // 此时 [s,s + n) 与当前字符串对象重叠.
        std::string saver(s,n);
        return replace(i1,i2,saver.data(),saver.size());
    }

    if (n1 > n2) {
        memcpy(const_cast<char*>(i1),s,n2);
        erase(i1 + n2, i2);
    } else {
        memcpy(const_cast<char*>(i1),s,n1);
        insert(i2, s + n1, n2 - n1);
    }
    return ;
}


