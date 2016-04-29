#include "cipher/memxor.h"

namespace {

constexpr unsigned

#define WORD_T_THRESH 16

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
  unsigned offset = ALIGN_OFFSET (src);
  word_t s0, s1;

  assert (n > 0);
  shl = CHAR_BIT * offset;
  shr = CHAR_BIT * (sizeof(word_t) - offset);

  src_word = (const word_t *) ((uintptr_t) src & -sizeof(word_t));

  /* Read top offset bytes, in native byte order. */
  READ_PARTIAL (s0, (unsigned char *) &src_word[n], offset);
#ifdef WORDS_BIGENDIAN
  s0 <<= shr; /* FIXME: Eliminate this shift? */
#endif

  /* Do n-1 regular iterations */
  if (n & 1)
    s1 = s0;
  else
    {
      n--;
      s1 = src_word[n];
      dst[n] ^= MERGE (s1, shl, s0, shr);
    }

  assert (n & 1);
  while (n > 2)
    {
      n -= 2;
      s0 = src_word[n+1];
      dst[n+1] ^= MERGE(s0, shl, s1, shr);
      s1 = src_word[n]; /* FIXME: Overread on last iteration */
      dst[n] ^= MERGE(s1, shl, s0, shr);
    }
  assert (n == 1);
  /* Read low wordsize - offset bytes */
  READ_PARTIAL (s0, src, sizeof(word_t) - offset);
#ifndef WORDS_BIGENDIAN
  s0 <<= shl; /* FIXME: eliminate shift? */
#endif /* !WORDS_BIGENDIAN */

  dst[0] ^= MERGE(s0, shl, s1, shr);
}

/* Performance, Intel SU1400 (x86_64): 0.25 cycles/byte aligned, 0.45
   cycles/byte unaligned. */

/* XOR LEN bytes starting at SRCADDR onto DESTADDR. Result undefined
   if the source overlaps with the destination. Return DESTADDR. */
void *
memxor(void *dst_in, const void *src_in, size_t n)
{
  unsigned char *dst = dst_in;
  const unsigned char *src = src_in;

  if (n >= WORD_T_THRESH)
    {
      unsigned i;
      unsigned offset;
      size_t nwords;
      /* There are at least some bytes to compare.  No need to test
     for N == 0 in this alignment loop.  */
      for (i = ALIGN_OFFSET(dst + n); i > 0; i--)
    {
      n--;
      dst[n] ^= src[n];
    }
      offset = ALIGN_OFFSET(src + n);
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

}
