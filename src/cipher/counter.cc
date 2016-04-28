#include "cipher/counter.h"
#include "exception/errno_exception.h"

namespace pp_qq {

void Counter::Increase(void *ptr,size_t n,int step) const
{
    if (step < 0)
        THROW(EINVAL,"不支持减法!step: %d",step);

    if (step > 0) {
        unsigned char *inout = static_cast<unsigned char*>(ptr);
        auto s = n;

        for (int i = 0; i < step; ++i)
            for (int j = s - 1, carry = 1; j >= 0 && carry; j--)
                carry = !++inout[j];
    }
    return ;
}

void Counter::Init(void *ptr,size_t n) const
{
    memset(ptr,0,n);
    return ;
}



}
