#include <memory.h>

#include <algorithm>
#include "common/utils.h"

int MemCompare(const void *l,size_t lsize,const void *r,size_t rsize) noexcept
{
    if (l == nullptr)
        lsize = 0;
    if (r == nullptr)
        rsize = 0;

    int memcmp_ret = 0;
    size_t min_size = std::min(lsize,rsize);
    if (min_size != 0) {
        memcmp_ret = memcmp(l,r,min_size);
        if (memcmp_ret != 0)
            return memcmp_ret;
    }

    if (lsize == rsize)
        return 0;
    if (lsize < rsize)
        return -1;
    return 1;
}

int GetUtf8LetterNumber(const char *src) noexcept {
    int i = 0, j = 0;  
    while (src[i]) {  
        if ((src[i] & 0xc0) != 0x80) 
            ++j;  
        ++i;
    }  
    return j; 
}
