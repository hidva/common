#ifndef ORG_PP_QQ_COMMON_CIPHER_MEMXOR_H
#define ORG_PP_QQ_COMMON_CIPHER_MEMXOR_H

#include <stddef.h>

void Memxor(void *dst,const void *ptr,size_t n);

void Memxor(void *dst,const void *a,const void *b,size_t n);

#endif // ORG_PP_QQ_COMMON_CIPHER_MEMXOR_H
