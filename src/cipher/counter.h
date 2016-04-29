#ifndef ORG_PP_QQ_COMMON_CIPHER_COUNTER_H_
#define ORG_PP_QQ_COMMON_CIPHER_COUNTER_H_

#include <stddef.h>
#include <string.h>

namespace pp_qq {

struct Counter {
    Counter() = default;

    inline void Skip(int n) noexcept;

    inline void Begin(void *ptr,size_t n) const;
    inline void Inc(void *ptr,size_t n) const;

    virtual void Init(void *ptr,size_t n) const;
    virtual void Increase(void *ptr,size_t n,int step) const;
private:
    int skip_off_ = 0;
};


void Counter::Skip(int n) noexcept
{
    skip_off_ = n;
    return ;
}

void Counter::Begin(void *ptr,size_t n) const
{
    Init(ptr,n);
    if (skip_off_ != 0)
        Increase(ptr,n,skip_off_);
    return ;
}

void Counter::Inc(void *ptr,size_t n) const
{
    Increase(ptr,n,1);
    return ;
}


}


#endif // ORG_PP_QQ_COMMON_CIPHER_COUNTER_H_
