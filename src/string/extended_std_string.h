#ifndef ORG_PP_QQ_COMMON_STRING_EXTENDED_STD_STRING_H
#define ORG_PP_QQ_COMMON_STRING_EXTENDED_STD_STRING_H

#include <string>

struct ExtendedStdString : public std::string {
    using std::string::string;

    inline ExtendedStdString();

    inline ExtendedStdString(ExtendedStdString &&);
    inline ExtendedStdString(const ExtendedStdString &);

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



#endif // ORG_PP_QQ_COMMON_STRING_EXTENDED_STD_STRING_H
