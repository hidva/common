/* memxor.c

   Copyright (C) 2010, 2014 Niels Möller

   This file is part of GNU Nettle.

   GNU Nettle is free software: you can redistribute it and/or
   modify it under the terms of either:

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at your
       option) any later version.

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at your
       option) any later version.

   or both in parallel, as here.

   GNU Nettle is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see http://www.gnu.org/licenses/.
*/

/* Implementation inspired by memcmp in glibc, contributed to the FSF
   by Torbjorn Granlund.
 */

#include "cipher/memxor.h"
#include "common/inline_utils.h"
#include "exception/errno_exception.h"

#include <stdint.h>
#include <endian.h>
#include <assert.h>

namespace {

#ifndef NDEBUG
#define NDEBUG 1
#endif

#undef PP_QQ_MEMXOR_WORDS_BIGENDIAN

#if __BYTE_ORDER == __LITTLE_ENDIAN
#undef PP_QQ_MEMXOR_WORDS_BIGENDIAN
#elif __BYTE_ORDER == __BIG_ENDIAN
#define PP_QQ_MEMXOR_WORDS_BIGENDIAN
#else
#error 无法判断当前机器大小端模式
#endif

#ifndef PP_QQ_MEMXOR_WORDS_BIGENDIAN
#define PP_QQ_MEMXOR_MERGE(w0, sh_1, w1, sh_2) \
  (((w0) >> (sh_1)) | ((w1) << (sh_2)))
#else
#define PP_QQ_MEMXOR_MERGE(w0, sh_1, w1, sh_2) \
  (((w0) << (sh_1)) | ((w1) >> (sh_2)))
#endif

#ifndef PP_QQ_MEMXOR_WORDS_BIGENDIAN
#define PP_QQ_MEMXOR_READ_PARTIAL(r,p,n) do {			\
    word_t _rp_x;					\
    unsigned _rp_i;					\
    for (_rp_i = (n), _rp_x = (p)[--_rp_i]; _rp_i > 0;)	\
      _rp_x = (_rp_x << kCharBit) | (p)[--_rp_i];	\
    (r) = _rp_x;					\
  } while (0)
#else
#define PP_QQ_MEMXOR_READ_PARTIAL(r,p,n) do {			\
    word_t _rp_x;						\
    unsigned _rp_i;						\
    for (_rp_x = (p)[0], _rp_i = 1; _rp_i < (n); _rp_i++)	\
      _rp_x = (_rp_x << kCharBit) | (p)[_rp_i];			\
    (r) = _rp_x;						\
  } while (0)
#endif


/* 按照我的理解,本来只需要
 *      typedef unsigned long word_t
 * 就行,这样在 32 bit机器上,word_t 也是 32 位;在 64 位机器上,word_t 也是 64 位.
 *
 * 但是在 MS Windows 上发现,即使是 64 位机器,unsigned long 也是 32 位;所以只能.
 */
#if defined(__x86_64__) || defined(__arch64__)
/* Including on M$ windows, where unsigned long is only 32 bits */
typedef uint64_t word_t;
#else
typedef unsigned long int word_t;
#endif

constexpr unsigned int kCharBit = 8;
constexpr unsigned int kWordTThresh = 16;

inline unsigned int AlignOffset(const void *p) noexcept
{
    return ((uintptr_t) (p) % sizeof(word_t));
}


/* XOR word-aligned areas. n is the number of words, not bytes. */
static void
memxor_common_alignment (word_t *dst, const word_t *src, size_t n)
{
  /* FIXME: Require n > 0? */
  /* FIXME: Unroll four times, like memcmp? Probably not worth the
     effort. */

  if (n & 1)
    {
      n--;
      dst[n] ^= src[n];
    }
  while (n >= 2)
    {
      n -= 2;
      dst[n+1] ^= src[n+1];
      dst[n] ^= src[n];
    }
}

/* XOR *un-aligned* src-area onto aligned dst area. n is number of
   words, not bytes. Assumes we can read complete words at the start
   and end of the src operand. */
static void
memxor_different_alignment (word_t *dst, const unsigned char *src, size_t n)
{
  int shl, shr;
  const word_t *src_word;
  unsigned offset = AlignOffset (src);
  word_t s0, s1;

  assert (n > 0);
  shl = kCharBit * offset;
  shr = kCharBit * (sizeof(word_t) - offset);

  src_word = (const word_t *) ((uintptr_t) src & -sizeof(word_t));

  /* Read top offset bytes, in native byte order. */
  PP_QQ_MEMXOR_READ_PARTIAL (s0, (unsigned char *) &src_word[n], offset);
#ifdef PP_QQ_MEMXOR_WORDS_BIGENDIAN
  s0 <<= shr; /* FIXME: Eliminate this shift? */
#endif

  /* Do n-1 regular iterations */
  if (n & 1)
    s1 = s0;
  else
    {
      n--;
      s1 = src_word[n];
      dst[n] ^= PP_QQ_MEMXOR_MERGE (s1, shl, s0, shr);
    }

  assert (n & 1);
  while (n > 2)
    {
      n -= 2;
      s0 = src_word[n+1];
      dst[n+1] ^= PP_QQ_MEMXOR_MERGE(s0, shl, s1, shr);
      s1 = src_word[n]; /* FIXME: Overread on last iteration */
      dst[n] ^= PP_QQ_MEMXOR_MERGE(s1, shl, s0, shr);
    }
  assert (n == 1);
  /* Read low wordsize - offset bytes */
  PP_QQ_MEMXOR_READ_PARTIAL (s0, src, sizeof(word_t) - offset);
#ifndef PP_QQ_MEMXOR_WORDS_BIGENDIAN
  s0 <<= shl; /* FIXME: eliminate shift? */
#endif /* !PP_QQ_MEMXOR_WORDS_BIGENDIAN */

  dst[0] ^= PP_QQ_MEMXOR_MERGE(s0, shl, s1, shr);
}

/* Performance, Intel SU1400 (x86_64): 0.25 cycles/byte aligned, 0.45
   cycles/byte unaligned. */

/* XOR LEN bytes starting at SRCADDR onto DESTADDR. Result undefined
   if the source overlaps with the destination. Return DESTADDR. */
void *
memxor(void *dst_in, const void *src_in, size_t n)
{
  unsigned char *dst = static_cast<unsigned char*>(dst_in);
  const unsigned char *src = static_cast<const unsigned char*>(src_in);

  if (n >= kWordTThresh)
    {
      unsigned i;
      unsigned offset;
      size_t nwords;
      /* There are at least some bytes to compare.  No need to test
     for N == 0 in this alignment loop.  */
      for (i = AlignOffset(dst + n); i > 0; i--)
    {
      n--;
      dst[n] ^= src[n];
    }
      offset = AlignOffset(src + n);
      nwords = n / sizeof (word_t);
      n %= sizeof (word_t);

      if (offset)
    memxor_different_alignment ((word_t *) (dst+n), src+n, nwords);
      else
    memxor_common_alignment ((word_t *) (dst+n),
                 (const word_t *) (src+n), nwords);
    }
  while (n > 0)
    {
      n--;
      dst[n] ^= src[n];
    }

  return dst;
}

/* XOR word-aligned areas. n is the number of words, not bytes. */
static void
memxor3_common_alignment (word_t *dst,
              const word_t *a, const word_t *b, size_t n)
{
  /* FIXME: Require n > 0? */
  if (n & 1)
    {
      n--;
      dst[n] = a[n] ^ b[n];
    }
  while (n > 0)
    {
      n -= 2;
      dst[n+1] = a[n+1] ^ b[n+1];
      dst[n] = a[n] ^ b[n];
    }
}

static void
memxor3_different_alignment_b (word_t *dst,
                   const word_t *a, const unsigned char *b,
                   unsigned offset, size_t n)
{
  int shl, shr;
  const word_t *b_word;

  word_t s0, s1;

  assert (n > 0);

  shl = kCharBit * offset;
  shr = kCharBit * (sizeof(word_t) - offset);

  b_word = (const word_t *) ((uintptr_t) b & -sizeof(word_t));

  /* Read top offset bytes, in native byte order. */
  PP_QQ_MEMXOR_READ_PARTIAL (s0, (unsigned char *) &b_word[n], offset);
#ifdef PP_QQ_MEMXOR_WORDS_BIGENDIAN
  s0 <<= shr;
#endif

  if (n & 1)
    s1 = s0;
  else
    {
      n--;
      s1 = b_word[n];
      dst[n] = a[n] ^ PP_QQ_MEMXOR_MERGE (s1, shl, s0, shr);
    }

  while (n > 2)
    {
      n -= 2;
      s0 = b_word[n+1];
      dst[n+1] = a[n+1] ^ PP_QQ_MEMXOR_MERGE(s0, shl, s1, shr);
      s1 = b_word[n];
      dst[n] = a[n] ^ PP_QQ_MEMXOR_MERGE(s1, shl, s0, shr);
    }
  assert (n == 1);
  /* Read low wordsize - offset bytes */
  PP_QQ_MEMXOR_READ_PARTIAL (s0, b, sizeof(word_t) - offset);
#ifndef PP_QQ_MEMXOR_WORDS_BIGENDIAN
  s0 <<= shl;
#endif /* !PP_QQ_MEMXOR_WORDS_BIGENDIAN */

  dst[0] = a[0] ^ PP_QQ_MEMXOR_MERGE(s0, shl, s1, shr);
}

static void
memxor3_different_alignment_ab (word_t *dst,
                const unsigned char *a, const unsigned char *b,
                unsigned offset, size_t n)
{
  int shl, shr;
  const word_t *a_word;
  const word_t *b_word;

  word_t s0, s1, t;

  assert (n > 0);

  shl = kCharBit * offset;
  shr = kCharBit * (sizeof(word_t) - offset);

  a_word = (const word_t *) ((uintptr_t) a & -sizeof(word_t));
  b_word = (const word_t *) ((uintptr_t) b & -sizeof(word_t));

  /* Read top offset bytes, in native byte order. */
  PP_QQ_MEMXOR_READ_PARTIAL (s0, (unsigned char *) &a_word[n], offset);
  PP_QQ_MEMXOR_READ_PARTIAL (t,  (unsigned char *) &b_word[n], offset);
  s0 ^= t;
#ifdef PP_QQ_MEMXOR_WORDS_BIGENDIAN
  s0 <<= shr;
#endif

  if (n & 1)
    s1 = s0;
  else
    {
      n--;
      s1 = a_word[n] ^ b_word[n];
      dst[n] = PP_QQ_MEMXOR_MERGE (s1, shl, s0, shr);
    }

  while (n > 2)
    {
      n -= 2;
      s0 = a_word[n+1] ^ b_word[n+1];
      dst[n+1] = PP_QQ_MEMXOR_MERGE(s0, shl, s1, shr);
      s1 = a_word[n] ^ b_word[n];
      dst[n] = PP_QQ_MEMXOR_MERGE(s1, shl, s0, shr);
    }
  assert (n == 1);
  /* Read low wordsize - offset bytes */
  PP_QQ_MEMXOR_READ_PARTIAL (s0, a, sizeof(word_t) - offset);
  PP_QQ_MEMXOR_READ_PARTIAL (t,  b, sizeof(word_t) - offset);
  s0 ^= t;
#ifndef PP_QQ_MEMXOR_WORDS_BIGENDIAN
  s0 <<= shl;
#endif /* !PP_QQ_MEMXOR_WORDS_BIGENDIAN */

  dst[0] = PP_QQ_MEMXOR_MERGE(s0, shl, s1, shr);
}

static void
memxor3_different_alignment_all (word_t *dst,
                 const unsigned char *a, const unsigned char *b,
                 unsigned a_offset, unsigned b_offset,
                 size_t n)
{
  int al, ar, bl, br;
  const word_t *a_word;
  const word_t *b_word;

  word_t a0, a1, b0, b1;

  al = kCharBit * a_offset;
  ar = kCharBit * (sizeof(word_t) - a_offset);
  bl = kCharBit * b_offset;
  br = kCharBit * (sizeof(word_t) - b_offset);

  a_word = (const word_t *) ((uintptr_t) a & -sizeof(word_t));
  b_word = (const word_t *) ((uintptr_t) b & -sizeof(word_t));

  /* Read top offset bytes, in native byte order. */
  PP_QQ_MEMXOR_READ_PARTIAL (a0, (unsigned char *) &a_word[n], a_offset);
  PP_QQ_MEMXOR_READ_PARTIAL (b0, (unsigned char *) &b_word[n], b_offset);
#ifdef PP_QQ_MEMXOR_WORDS_BIGENDIAN
  a0 <<= ar;
  b0 <<= br;
#endif

  if (n & 1)
    {
      a1 = a0; b1 = b0;
    }
  else
    {
      n--;
      a1 = a_word[n];
      b1 = b_word[n];

      dst[n] = PP_QQ_MEMXOR_MERGE (a1, al, a0, ar) ^ PP_QQ_MEMXOR_MERGE (b1, bl, b0, br);
    }
  while (n > 2)
    {
      n -= 2;
      a0 = a_word[n+1]; b0 = b_word[n+1];
      dst[n+1] = PP_QQ_MEMXOR_MERGE(a0, al, a1, ar) ^ PP_QQ_MEMXOR_MERGE(b0, bl, b1, br);
      a1 = a_word[n]; b1 = b_word[n];
      dst[n] = PP_QQ_MEMXOR_MERGE(a1, al, a0, ar) ^ PP_QQ_MEMXOR_MERGE(b1, bl, b0, br);
    }
  assert (n == 1);
  /* Read low wordsize - offset bytes */
  PP_QQ_MEMXOR_READ_PARTIAL (a0, a, sizeof(word_t) - a_offset);
  PP_QQ_MEMXOR_READ_PARTIAL (b0, b, sizeof(word_t) - b_offset);
#ifndef PP_QQ_MEMXOR_WORDS_BIGENDIAN
  a0 <<= al;
  b0 <<= bl;
#endif /* !PP_QQ_MEMXOR_WORDS_BIGENDIAN */

  dst[0] = PP_QQ_MEMXOR_MERGE(a0, al, a1, ar) ^ PP_QQ_MEMXOR_MERGE(b0, bl, b1, br);
}

/* Current implementation processes data in descending order, to
   support overlapping operation with one of the sources overlapping
   the start of the destination area. This feature is used only
   internally by cbc decrypt, and it is not advertised or documented
   to nettle users. */
void *
memxor3(void *dst_in, const void *a_in, const void *b_in, size_t n)
{
  unsigned char *dst = static_cast<unsigned char*>(dst_in);
  const unsigned char *a = static_cast<const unsigned char*>(a_in);
  const unsigned char *b = static_cast<const unsigned char*>(b_in);

  if (n >= kWordTThresh)
    {
      unsigned i;
      unsigned a_offset;
      unsigned b_offset;
      size_t nwords;

      for (i = AlignOffset(dst + n); i > 0; i--)
    {
      n--;
      dst[n] = a[n] ^ b[n];
    }

      a_offset = AlignOffset(a + n);
      b_offset = AlignOffset(b + n);

      nwords = n / sizeof (word_t);
      n %= sizeof (word_t);

      if (a_offset == b_offset)
    {
      if (!a_offset)
        memxor3_common_alignment((word_t *) (dst + n),
                     (const word_t *) (a + n),
                     (const word_t *) (b + n), nwords);
      else
        memxor3_different_alignment_ab((word_t *) (dst + n),
                       a + n, b + n, a_offset,
                       nwords);
    }
      else if (!a_offset)
    memxor3_different_alignment_b((word_t *) (dst + n),
                      (const word_t *) (a + n), b + n,
                      b_offset, nwords);
      else if (!b_offset)
    memxor3_different_alignment_b((word_t *) (dst + n),
                      (const word_t *) (b + n), a + n,
                      a_offset, nwords);
      else
    memxor3_different_alignment_all((word_t *) (dst + n), a + n, b + n,
                    a_offset, b_offset, nwords);

    }
  while (n-- > 0)
    dst[n] = a[n] ^ b[n];

  return dst;
}


}



void Memxor(void *dst,const void *ptr,size_t n)
{
    if (dst != ptr && IsOverlap(dst,n,ptr,n))
        THROW(EINVAL,"输入,输出重叠!");
    memxor(dst,ptr,n);
    return ;
}

void Memxor(void *dst,const void *a,const void *b,size_t n)
{
    if (IsOverlap(a,n,b,n) || IsOverlap(a,n,dst,n) || IsOverlap(dst,n,b,n))
        THROW(EINVAL,"输入,输出重叠");
    memxor3(dst,a,b,n);
    return ;
}




