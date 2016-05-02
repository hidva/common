
#include <openssl/aes.h>
#include "log/pp_qq_log.h"
#include "hex_coding/hex_coding.h"
#include "string/extended_std_string.h"
#include "exception/errno_exception.h"

inline const unsigned char* cucp(const void *ptr) noexcept
{
    return static_cast<const unsigned char*>(ptr);
}

inline unsigned char* ucp(void *ptr) noexcept
{
    return static_cast<unsigned char*>(ptr);
}

int
main(int ,char **)
{
    const ExtendedStdString plaintext = HexDecode<ExtendedStdString>("e682a8e5a5bde5958a20e69c8be58f8b");
    PP_QQ_CHECK(plaintext.size() == 16);

    const ExtendedStdString key16 = HexDecode<ExtendedStdString>("01020304050607080910111213141516");
    const ExtendedStdString key24 = HexDecode<ExtendedStdString>("010203040506070809101112131415161718192021222324");
    const ExtendedStdString key32 = HexDecode<ExtendedStdString>("0102030405060708091011121314151617181920212223242526272829303132");
    PP_QQ_CHECK(key16.size() == 16);
    PP_QQ_CHECK(key24.size() == 24);
    PP_QQ_CHECK(key32.size() == 32);

    ExtendedStdString ciphertext16(16,'x');
    ExtendedStdString ciphertext24(16,'x');
    ExtendedStdString ciphertext32(16,'x');

    AES_KEY aes_ctx;

    auto GetCipherText = [&] (const ExtendedStdString &key,ExtendedStdString &ciphertext) {
        ciphertext.resize(16);

        PP_QQ_CHECK(AES_set_encrypt_key(cucp(key.const_raw_data()),key.size() * 8,&aes_ctx) == 0);
        AES_encrypt(cucp(plaintext.const_raw_data()),ucp(ciphertext.raw_data()),&aes_ctx);

        ExtendedStdString buf_for_test = ciphertext;
        PP_QQ_CHECK(AES_set_decrypt_key(cucp(key.const_raw_data()),key.size() * 8,&aes_ctx) == 0);
        AES_decrypt(cucp(buf_for_test.const_raw_data()),ucp(buf_for_test.raw_data()),&aes_ctx);

        PP_QQ_CHECK(plaintext == buf_for_test);
        return ;
    };

    GetCipherText(key16,ciphertext16);
    GetCipherText(key24,ciphertext24);
    GetCipherText(key32,ciphertext32);

    auto PrintCipherText = [] (const ExtendedStdString &ciphertext,const char *desc) {
        PP_QQ_LOG_I("%s: %s",desc,HexEncode<ExtendedStdString>(ciphertext.const_raw_data(),ciphertext.size()).cdata());
        return ;
    };
    PrintCipherText(ciphertext16,"ciphertext16");
    PrintCipherText(ciphertext24,"ciphertext24");
    PrintCipherText(ciphertext32,"ciphertext32");
    return 0;
}


