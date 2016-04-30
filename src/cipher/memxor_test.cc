
#include "cipher/memxor.h"
#include "gtest/gtest.h"
#include "hex_coding/hex_coding.h"
#include "string/extended_std_string.h"

#include <stdint.h>

#define MAX_SIZE 512
#define ALIGN_SIZE 16


namespace {

uint8_t *
set_align(uint8_t *buf, unsigned align)
{
  unsigned offset;
  /* An extra redzone char at the beginning */
  buf++;
  offset = (uintptr_t) (buf) % ALIGN_SIZE;

  if (offset < align)
    buf += (align - offset);
  else if (offset > align)
    buf += (align + ALIGN_SIZE - offset);

  return buf;
}

static void
test_memxor (const uint8_t *a, const uint8_t *b, const uint8_t *c,
         size_t size, unsigned align_dst, unsigned align_src)
{
  uint8_t dst_buf[MAX_SIZE + ALIGN_SIZE + 1];
  uint8_t src_buf[MAX_SIZE + ALIGN_SIZE + 1];

  uint8_t *dst = set_align (dst_buf, align_dst);
  uint8_t *src = set_align (src_buf, align_src);

  memcpy (src, b, size);
  memcpy (dst, a, size);
  dst[-1] = 17;
  dst[size] = 17;

  Memxor (dst, src, size);
  EXPECT_EQ(0,memcmp(dst,c,size));
  EXPECT_EQ(17,dst[-1]);
  EXPECT_EQ(17,dst[size]);

  return ;
}

static void
test_memxor3 (const uint8_t *ain, const uint8_t *bin, const uint8_t *c,
          size_t size, unsigned align_dst, unsigned align_a, unsigned align_b)
{
  uint8_t dst_buf[MAX_SIZE + ALIGN_SIZE + 1];
  uint8_t a_buf[MAX_SIZE + ALIGN_SIZE + 1];
  uint8_t b_buf[MAX_SIZE + ALIGN_SIZE + 1];

  uint8_t *dst = set_align (dst_buf, align_dst);
  uint8_t *a = set_align (a_buf, align_a);
  uint8_t *b = set_align (b_buf, align_b);

  memcpy (a, ain, size);
  memcpy (b, bin, size);
  memset (dst, 0, size);
  dst[-1] = 17;
  dst[size] = 17;

  Memxor (dst, a, b, size);
  EXPECT_EQ(0,memcmp(dst,c,size));
  EXPECT_EQ(17,dst[-1]);
  EXPECT_EQ(17,dst[size]);
  return ;
}

}


TEST(MemxorTest,test1)
{
    const char *a_hex_str = "ecc8737f38f2f9e886b9d84c42a9c7ef"
                            "27a5086049c6be97c5cc6c353981b367"
                            "f8b4397b951e3b2f35749fe125884fa6"
                            "9361c97aab1c6cce494efb5a1f108411"
                            "21dc6386e81b24102f04c29de0ca1135"
                            "c9f96f2ebb5b2e2d8cb45df950c4755a"
                            "362b7ead4b930010cbc6983466221ba8"
                            "c0b8d7ac7ec3b7006bdb1a3b599f3e76"
                            "a7e66a29ee1fb98c60a66c9e0a1d9c49"
                            "6367afc7362d6ae1f879944317e2b1a1"
                            "ff1cc03c9e2728caa1f6598f5a61bd56"
                            "0826effcf3499da7119249b6fd643cd4"
                            "2e7c74b0f775fda4a56171381e8520bf"
                            "f17de57adecc36b69eceee6ed448f592"
                            "be77a67a1b91a5b362fab868dcb046f6"
                            "394b5335b2eaa351fc4456e435bb9c54";
    const char *b_hex_str = "cac458adfe87e2266cb0ce3dcfa5cb3b"
                            "963d00345811bb9eacf4675b7464f800"
                            "4b1bcff2b2fa5dd00576aea6888b8150"
                            "bcba48f149bc33d2e138b0d0a29b486e"
                            "f7e143c6f99595966aaa4493b0bea6f8"
                            "1d778513a3bfec7e70cfe6a7e31ad041"
                            "5fe3371b63aba991dab9a3db66310ebc"
                            "24c2765da722a1312fc4d3661f2e3388"
                            "7e5b26d57b34bf4c655d19dad1335362"
                            "2fbc0d5dcc68c811ef735c20352986ef"
                            "f47ac5c9afa77f5a20da6dd3eb9dfb34"
                            "0cdbf792caf0d63361d908daa4c0f2a9"
                            "be7a573e3b8d161c47fc19bee47d7edc"
                            "e5f00daef64cbbb4a081e1f0381833d8"
                            "30d302ffeed618873390d6b20048ac32"
                            "9c6b2981a224dcc16b1feebe15834b1a";
    const char *c_hex_str = "260c2bd2c6751bceea0916718d0c0cd4"
                            "b198085411d7050969380b6e4de54b67"
                            "b3aff68927e466ff30023147ad03cef6"
                            "2fdb818be2a05f1ca8764b8abd8bcc7f"
                            "d63d2040118eb18645ae860e5074b7cd"
                            "d48eea3d18e4c253fc7bbb5eb3dea51b"
                            "69c849b62838a981117f3bef00131514"
                            "e47aa1f1d9e11631441fc95d46b10dfe"
                            "d9bd4cfc952b06c005fb7544db2ecf2b"
                            "4cdba29afa45a2f0170ac86322cb374e"
                            "0b6605f531805790812c345cb1fc4662"
                            "04fd186e39b94b94704b416c59a4ce7d"
                            "9006238eccf8ebb8e29d6886faf85e63"
                            "148de8d428808d023e4f0f9eec50c64a"
                            "8ea4a485f547bd34516a6edadcf8eac4"
                            "a5207ab410ce7f90975bb85a2038d74e";

    ExtendedStdString a_hex = HexDecode<ExtendedStdString>(a_hex_str,strlen(a_hex_str));
    ExtendedStdString b_hex = HexDecode<ExtendedStdString>(b_hex_str,strlen(b_hex_str));
    ExtendedStdString c_hex = HexDecode<ExtendedStdString>(c_hex_str,strlen(c_hex_str));

    const uint8_t *a = reinterpret_cast<const uint8_t*>(a_hex.const_raw_data());
    const uint8_t *b = reinterpret_cast<const uint8_t*>(b_hex.const_raw_data());
    const uint8_t *c = reinterpret_cast<const uint8_t*>(c_hex.const_raw_data());

  const int size[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 23, 24, 25, 30, 31, 32, 33, 34, 35, 36, 37,
    250, 251, 252, 253,254, 255, 256, -1
  };

  unsigned i, align_dst, align_a, align_b;
  for (i = 0; size[i] >= 0; i++)
    for (align_dst = 0; align_dst < ALIGN_SIZE; align_dst++)
      for (align_a = 0; align_a < ALIGN_SIZE; align_a++)
    {
      test_memxor (a, b, c, size[i], align_dst, align_a);
      for (align_b = 0; align_b < ALIGN_SIZE; align_b++)
        test_memxor3 (a, b, c, size[i], align_dst, align_a, align_b);
    }
}



