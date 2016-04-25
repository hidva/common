#ifndef ORG_PP_QQ_COMMON_STRING_EXTENDED_STD_STRING_H
#define ORG_PP_QQ_COMMON_STRING_EXTENDED_STD_STRING_H

#include <string>

struct ExtendedStdString : public std::string {
    using std::string::string;

    inline ExtendedStdString();

    inline ExtendedStdString(ExtendedStdString &&);
    inline ExtendedStdString(const ExtendedStdString &);

    /* 这里要指定 explicit,不然 std::string 类型就可以自动转化为 ExtendedStdString 类型.
     * 但是 ExtendedStdString 类型已经可以自动转化为 std::string 类型了,所以可能会造成很
     * 多二义性.
     *
     * 相比 std::string 类型自动转化为 ExtendedStdString 类型;ExtendedStdString 更适合
     * 转化为 std::string 类型.如下:
     *
     *    std::string str;
     *    // 此时会 ExtendedStdString tmp(str); estr = tmp;会拷贝一份 str.
     *    const ExtendedStdString &estr = str;
     *
     *    ExtendedStdString estr;
     *    // 此时并不会有啥子拷贝.
     *    const std::string &str = estr;
     */
    explicit inline ExtendedStdString(const std::string &);
    explicit inline ExtendedStdString(std::string &&);

    using std::string::operator=;

    inline const value_type& cfront() const;
    inline const value_type& cback() const;
    inline const_reference cat(size_type n) const;

    inline value_type* data();
    inline const value_type* data() const;
    inline const value_type* cdata() const;

    inline value_type* raw_data();
    inline const value_type* raw_data() const;
    inline const value_type* const_raw_data() const;
};



ExtendedStdString::ExtendedStdString()
{
}

ExtendedStdString::ExtendedStdString(const std::string &other):
    std::string(other)
{
}

ExtendedStdString::ExtendedStdString(std::string &&other):
    std::string(std::move(other))
{
}

ExtendedStdString::ExtendedStdString(ExtendedStdString &&other):
    std::string{static_cast<std::string&&>(other)}
{
}


ExtendedStdString::ExtendedStdString(const ExtendedStdString &other):
    std::string{other}
{
}



auto ExtendedStdString::cfront() const -> const value_type&
{
    return front();
}


auto ExtendedStdString::cback() const -> const value_type&
{
    return back();
}


auto ExtendedStdString::cat(size_type n) const -> const_reference
{
    return at(n);
}


auto ExtendedStdString::data() -> value_type*
{
    // 若 std::string 采用 COW 机制,则 reserve(capacity()) 可以起到 detach 的效果.
    // 若 std::string 没采用 COW 机制,则也没啥影响
    reserve(capacity());
    return const_cast<value_type*>(cdata());
}


auto ExtendedStdString::data() const -> const value_type*
{
    return cdata();
}


auto ExtendedStdString::cdata() const -> const value_type*
{
    return c_str();
}


auto ExtendedStdString::raw_data() -> value_type*
{
    return data();
}


auto ExtendedStdString::raw_data() const -> const value_type*
{
    return data();
}


auto ExtendedStdString::const_raw_data() const -> const value_type*
{
    return cdata();
}




inline ExtendedStdString
operator+(const ExtendedStdString& lhs,const ExtendedStdString& rhs)
{
    return ExtendedStdString(static_cast<const std::string&>(lhs) + static_cast<const std::string&>(rhs));
}


inline ExtendedStdString
operator+(ExtendedStdString&& lhs,const ExtendedStdString& rhs)
{
    return ExtendedStdString(static_cast<std::string&&>(lhs) + static_cast<const std::string&>(rhs));
}


inline ExtendedStdString
operator+(const ExtendedStdString& lhs,ExtendedStdString&& rhs)
{
    return ExtendedStdString(static_cast<const std::string&>(lhs) + static_cast<std::string&&>(rhs));
}


inline ExtendedStdString
operator+(ExtendedStdString&& lhs,ExtendedStdString&& rhs)
{
    return ExtendedStdString(static_cast<std::string&&>(lhs) + static_cast<std::string&&>(rhs));
}


inline ExtendedStdString
operator+(const char* lhs,const ExtendedStdString& rhs)
{
    return ExtendedStdString(lhs + static_cast<const std::string&>(rhs));
}


inline ExtendedStdString
operator+(const char* lhs,ExtendedStdString&& rhs)
{
    return ExtendedStdString(lhs + static_cast<std::string&&>(rhs));
}


inline ExtendedStdString
operator+(char lhs,const ExtendedStdString& rhs)
{
    return ExtendedStdString(lhs + static_cast<const std::string&>(rhs));
}


inline ExtendedStdString
operator+(char lhs,ExtendedStdString&& rhs)
{
    return ExtendedStdString(lhs + static_cast<std::string&&>(rhs));
}


inline ExtendedStdString
operator+(const ExtendedStdString& lhs,const char* rhs)
{
    return ExtendedStdString(static_cast<const std::string&>(lhs) + rhs);
}


inline ExtendedStdString
operator+(ExtendedStdString&& lhs,const char* rhs)
{
    return ExtendedStdString(static_cast<std::string&&>(lhs) + rhs);
}


inline ExtendedStdString
operator+(const ExtendedStdString& lhs,char rhs)
{
    return ExtendedStdString(static_cast<const std::string&>(lhs) + rhs);
}


inline ExtendedStdString
operator+(ExtendedStdString&& lhs,char rhs)
{
    return ExtendedStdString(static_cast<std::string&&>(lhs) + rhs);
}

inline bool
operator==(const ExtendedStdString& lhs,const ExtendedStdString& rhs)
{
    return static_cast<const std::string&>(lhs) == static_cast<const std::string&>(rhs);
}



inline bool
operator==(const ExtendedStdString& lhs,const ExtendedStdString::value_type* rhs)
{
    return static_cast<const std::string&>(lhs) == rhs;
}

inline bool
operator==(const ExtendedStdString::value_type* lhs,const ExtendedStdString& rhs)
{
    return lhs == static_cast<const std::string&>(rhs);
}


inline bool
operator!=(const ExtendedStdString& lhs,const ExtendedStdString& rhs)
{
    return static_cast<const std::string&>(lhs) != static_cast<const std::string&>(rhs);
}


inline bool
operator!=(const ExtendedStdString::value_type* lhs,const ExtendedStdString& rhs)
{
    return lhs != static_cast<const std::string&>(rhs);
}


inline bool
operator!=(const ExtendedStdString& lhs,const ExtendedStdString::value_type* rhs)
{
    return static_cast<const std::string&>(lhs) != rhs;
}


inline bool
operator<(const ExtendedStdString& lhs,const ExtendedStdString& rhs)
{
    return static_cast<const std::string&>(lhs) < static_cast<const std::string&>(rhs);
}


inline bool
operator<(const ExtendedStdString& lhs,const ExtendedStdString::value_type* rhs)
{
    return static_cast<const std::string&>(lhs) < rhs;
}


inline bool
operator<(const ExtendedStdString::value_type* lhs,const ExtendedStdString& rhs)
{
    return lhs < static_cast<const std::string&>(rhs);
}


inline bool
operator>(const ExtendedStdString& lhs,const ExtendedStdString& rhs)
{
    return static_cast<const std::string&>(lhs) > static_cast<const std::string&>(rhs);
}


inline bool
operator>(const ExtendedStdString& lhs,const ExtendedStdString::value_type* rhs)
{
    return static_cast<const std::string&>(lhs) > rhs;
}


inline bool
operator>(const ExtendedStdString::value_type* lhs,const ExtendedStdString& rhs)
{
    return lhs > static_cast<const std::string&>(rhs);
}


inline bool
operator<=(const ExtendedStdString& lhs,const ExtendedStdString& rhs)
{
    return static_cast<const std::string&>(lhs) <= static_cast<const std::string&>(rhs);
}


inline bool
operator<=(const ExtendedStdString& lhs,const ExtendedStdString::value_type* rhs)
{
    return static_cast<const std::string&>(lhs) <= rhs;
}


inline bool
operator<=(const ExtendedStdString::value_type* lhs,const ExtendedStdString& rhs)
{
    return lhs <= static_cast<const std::string&>(rhs);
}


inline bool
operator>=(const ExtendedStdString& lhs,const ExtendedStdString& rhs)
{
    return static_cast<const std::string&>(lhs) >= static_cast<const std::string&>(rhs);
}


inline bool
operator>=(const ExtendedStdString& lhs,const ExtendedStdString::value_type* rhs)
{
    return static_cast<const std::string&>(lhs) >= rhs;
}


inline bool
operator>=(const ExtendedStdString::value_type* lhs,const ExtendedStdString& rhs)
{
    return lhs >= static_cast<const std::string&>(rhs);
}


inline void
swap(ExtendedStdString& lhs, ExtendedStdString& rhs)
{
    lhs.swap(rhs);
    return ;
}

#endif // ORG_PP_QQ_COMMON_STRING_EXTENDED_STD_STRING_H
