
#include "cipher/aes_cipher.h"
#include "cipher/ctr_cipher_mode.h"
#include "string/extended_std_string.h"
#include "cipher/counter.h"
#include "hex_coding/hex_coding.h"

#include "gtest/gtest.h"

#include <vector>

struct PlaintextCiphertextPair {
    ExtendedStdString plaintext;
    ExtendedStdString ciphertext;
};

/* 当前从三个方面测试 BlockCipher:
 * 1. 一次加密(或者解密)过程可以分为若干次 update() 与 1 次 final() 调用;但无论分成多少
 *    次 update() 调用,得到的结果都是相同的.
 * 2. BlockCipher 对象再经过一次加密(或解密)过程之后,其状态复原,即下一次对同样的明文(或
 *    密文)再一次走加密(或解密)过程,得到的结果一致.
 * 3. 若 BlockCipher 对象的实现支持原址加密(或解密),则应该也要测试这一块.
 */

TEST(AES256CTRCipherTest,test)
{
    /* 当前测试逻辑:
     *
     * 1. 设明文 P 的长度为 x * kBlockSize + y;其中 x 为 P 中数据块的数目,而 y 为 P
     *    中不足一块的内容的长度.
     * 2. 再设 P 可以经过 z 次 update() 与 1 次 final() 调用;其中 z 次 update() 时
     *    明文长度设置 A[0],A[1],...,A[z - 1];其中:
     *    - A[0],...,A[z - 2] 的值为 kBlockSize;
     *    - A[z - 1] = (x - (z - 1)) * kBlockSize;
     *
     *    final() 时明文长度为 y;在实现中将最后一次 Update() 与 final() 组合成一次 final()
     *    调用;
     */
    constexpr size_t kPlaintextBlockNumMax = 33;
//    constexpr size_t kPlaintextSizeMax = kPlaintextBlockNumMax * AESCipher::kBlockSize + (AESCipher::kBlockSize - 1);

    const ExtendedStdString key32 = HexDecode<ExtendedStdString>("0102030405060708091011121314151617181920212223242526272829303132");
    ASSERT_EQ(32,key32.size());
    AESCipher cipher;
    cipher.SetKey(key32.const_raw_data(),key32.size());

    pp_qq::Counter counter_impl;
    CTRCipherMode ctr_mode;
    ctr_mode.SetCounterImpl(&counter_impl);

    BlockCipher block_cipher(&cipher,&ctr_mode,nullptr);

    for (size_t x = 0; x <= kPlaintextBlockNumMax; ++x) {
        for (size_t y = 0; y < AESCipher::kBlockSize; ++y) {
            size_t const plaintext_size = x * AESCipher::kBlockSize + y;
            const ExtendedStdString expect_plaintext(plaintext_size,'x');
            ExtendedStdString expect_ciphertext;
            ASSERT_EQ(plaintext_size,expect_plaintext.size());

            // 测试1;
            for (int z = 0; z <= static_cast<int>(x); ++z) {
                ExtendedStdString ciphertext;

                for (int d = 0; d <= z - 2; ++d) {
                    auto size_before = block_cipher.size();
                    EXPECT_EQ(AESCipher::kBlockSize,block_cipher.EncryptUpdate(ciphertext,expect_plaintext.const_raw_data() + d * AESCipher::kBlockSize,AESCipher::kBlockSize));
                    EXPECT_EQ(AESCipher::kBlockSize,block_cipher.size() - size_before);
                }
                auto *left_ptr = expect_plaintext.const_raw_data();
                if (z - 1 > 0)
                    left_ptr += (z - 1) * AESCipher::kBlockSize;
                auto left_size = expect_plaintext.const_raw_data() + expect_plaintext.size() - left_ptr;
                {
                    auto size_before = ciphertext.size();
                    EXPECT_EQ(left_size,block_cipher.EncryptFinal(ciphertext,left_ptr,left_size));
                    EXPECT_EQ(left_size,ciphertext.size() - size_before);
                }
                ASSERT_EQ(expect_plaintext.size(),ciphertext.size());
                ASSERT_NE(expect_plaintext,ciphertext);

                if (expect_ciphertext.empty())
                    expect_ciphertext = ciphertext;
                else
                    EXPECT_EQ(expect_ciphertext,ciphertext);
            }

            // 测试 2,3
            for (int z = 0; z <= static_cast<int>(x); ++z) {
                ExtendedStdString ciphertext = expect_plaintext;
                char *iptr = ciphertext.raw_data();

                for (int d = 0; d <= z - 2; ++d) {
                    auto cipher_result = block_cipher.EncryptUpdate(iptr,AESCipher::kBlockSize,iptr,AESCipher::kBlockSize);
                    EXPECT_EQ(AESCipher::kBlockSize,cipher_result.input_size);
                    EXPECT_EQ(AESCipher::kBlockSize,cipher_result.output_size);
                    iptr += AESCipher::kBlockSize;
                }

                {
                    size_t left_size = ciphertext.const_raw_data() + ciphertext.size() - iptr;
                    auto cipher_result = block_cipher.EncryptFinal(iptr,left_size,iptr,left_size);
                    EXPECT_EQ(left_size,cipher_result.input_size);
                    EXPECT_EQ(left_size,cipher_result.output_size);
                }

                ASSERT_NE(expect_plaintext,ciphertext);
                ASSERT_FALSE(expect_ciphertext.empty());
                EXPECT_EQ(expect_ciphertext,ciphertext);
            }

            // 测试解密 1;
            for (int z = 0; z <= static_cast<int>(x); ++z) {
                ExtendedStdString plaintext;
                const char *iptr = expect_ciphertext.const_raw_data();

                for (int d = 0; d <= z - 2; ++d) {
                    size_t size_before = plaintext.size();
                    auto ret_size = block_cipher.DecryptUpdate(plaintext,iptr,AESCipher::kBlockSize);
                    EXPECT_EQ(AESCipher::kBlockSize,ret_size);
                    EXPECT_EQ(AESCipher::kBlockSize,plaintext.size() - size_before);
                    iptr += AESCipher::kBlockSize;
                }

                {
                    size_t left_size = expect_ciphertext.const_raw_data() + expect_ciphertext.size() - iptr;
                    size_t size_before = plaintext.size();
                    size_t ret_size = block_cipher.DecryptFinal(plaintext,iptr,left_size);
                    EXPECT_EQ(left_size,ret_size);
                    EXPECT_EQ(left_size,plaintext.size() - size_before);
                }

                EXPECT_EQ(expect_plaintext,plaintext);
            }

            // 测试解密 2,3;
            for (int z = 0; z <= static_cast<int>(x); ++z) {
                ExtendedStdString plaintext = expect_ciphertext;
                const char *iptr = plaintext.raw_data();

                for (int d = 0; d <= z - 2; ++d) {
                    auto cipher_result = block_cipher.DecryptUpdate(iptr,AESCipher::kBlockSize,iptr,AESCipher::kBlockSize);
                    EXPECT_EQ(AESCipher::kBlockSize,cipher_result.input_size);
                    EXPECT_EQ(AESCipher::kBlockSize,cipher_result.output_size);
                    iptr += AESCipher::kBlockSize;
                }

                {
                    size_t left_size = plaintext.const_raw_data() + plaintext.size() - iptr;
                    auto cipher_result = block_cipher.DecryptFinal(iptr,left_size,iptr,left_size);
                    EXPECT_EQ(left_size,cipher_result.input_size);
                    EXPECT_EQ(left_size,cipher_result.output_size);
                }

                EXPECT_EQ(expect_plaintext,plaintext);
            }
        }
    }

}
