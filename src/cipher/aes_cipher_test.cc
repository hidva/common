

#include "cipher/aes_cipher.h"
#include "hex_coding/hex_coding.h"
#include "string/extended_std_string.h"
#include "gtest/gtest.h"

struct AESCipherTest : public ::testing::Test {
    void SetUp() override;
    void TearDown() override;

    ExtendedStdString plaintext;
    ExtendedStdString key16,key24,key32;
    ExtendedStdString ciphertext16,ciphertext24,ciphertext32;

    AESCipher cipher;
};

void AESCipherTest::SetUp()
{
    plaintext = HexDecode<ExtendedStdString>("e682a8e5a5bde5958a20e69c8be58f8b");

    key16 = HexDecode<ExtendedStdString>("01020304050607080910111213141516");
    key24 = HexDecode<ExtendedStdString>("010203040506070809101112131415161718192021222324");
    key32 = HexDecode<ExtendedStdString>("0102030405060708091011121314151617181920212223242526272829303132");

    ciphertext16 = HexDecode<ExtendedStdString>("798D9A2404FF1F3CE1DC816394D1B894");
    ciphertext24 = HexDecode<ExtendedStdString>("20D4DBFA7D80B5EB42E5AE33D28BE136");
    ciphertext32 = HexDecode<ExtendedStdString>("C44C9C9AB7DF3756DB30CBA59213513E");
    return ;
}

void AESCipherTest::TearDown()
{
    return ;
}


TEST_F(AESCipherTest,GetBlockSize)
{
    EXPECT_EQ(16,cipher.GetBlockSize());
}

TEST_F(AESCipherTest,InitMode)
{
    EXPECT_ANY_THROW(cipher.InitMode(BlockCipherImpl::kDecryptMode));
    EXPECT_ANY_THROW(cipher.InitMode(BlockCipherImpl::kEncryptMode));
}

TEST_F(AESCipherTest,InitMode1)
{
    for (int i = 0; i <= 32; ++i) {
        ExtendedStdString key_tmp(i,'c');
        cipher.SetKey(key_tmp.const_raw_data(),key_tmp.size());

        if (i != 16 && i != 24 && i != 32) {
            EXPECT_ANY_THROW(cipher.InitMode(BlockCipherImpl::kDecryptMode));
            EXPECT_ANY_THROW(cipher.InitMode(BlockCipherImpl::kEncryptMode));
        } else {
            (cipher.InitMode(BlockCipherImpl::kDecryptMode));
            (cipher.InitMode(BlockCipherImpl::kEncryptMode));
        }
    }
}

TEST_F(AESCipherTest,EncryptDecrypt)
{
    auto Test = [&] (const ExtendedStdString &expect_ciphertext,const ExtendedStdString &key) {
        cipher.SetKey(key.const_raw_data(),key.size());
        cipher.InitMode(BlockCipherImpl::kEncryptMode);

        ExtendedStdString ciphertext(16,'x');
        cipher.Encrypt(ciphertext.raw_data(),plaintext.const_raw_data());
        EXPECT_EQ(expect_ciphertext,ciphertext);

        ciphertext = plaintext;
        cipher.Encrypt(ciphertext.raw_data(),ciphertext.const_raw_data());
        EXPECT_EQ(expect_ciphertext,ciphertext);

        ExtendedStdString buf (16,'x');
        cipher.InitMode(BlockCipherImpl::kDecryptMode);
        cipher.Decrypt(buf.raw_data(),expect_ciphertext.const_raw_data());
        EXPECT_EQ(plaintext,buf);

        buf = expect_ciphertext;
        cipher.Decrypt(buf.raw_data(),buf.const_raw_data());
        EXPECT_EQ(plaintext,buf);
    };
    
    for (int i = 0; i < 33; ++i) {        
        Test(ciphertext16,key16);
        Test(ciphertext24,key24);
        Test(ciphertext32,key32);
    }
}




