
#include "hex_coding/hex_coding.h"
#include "log/pp_qq_log.h"
#include "string/extended_std_string.h"

#include <openssl/aes.h>
#include "nettle/aes.h"

int
main(int ,char **)
{
    const char *key = "HelloWorldWorldHelloWorldHelloHW";
    const char *plaintext = "HelloWorldHelloW";

    {
        ExtendedStdString nettle_ciphertext(16,'x');
        PP_QQ_LOG_D("size: %zu",nettle_ciphertext.size());
        aes256_ctx ctx;
        aes256_set_encrypt_key(&ctx,reinterpret_cast<const uint8_t*>(key));
        aes256_encrypt(&ctx,nettle_ciphertext.size(),(uint8_t*)nettle_ciphertext.raw_data(),(const uint8_t*)plaintext);
        PP_QQ_LOG_D("nettle ciphertext: %s",HexEncode<ExtendedStdString>(nettle_ciphertext.const_raw_data(),nettle_ciphertext.size()).cdata());
    }

    {
        ExtendedStdString openssl_ciphertext(16,'c');
        AES_KEY aes_key;
        AES_set_encrypt_key((const unsigned char*)key,32 * 8,&aes_key);
        AES_encrypt((const unsigned char*)plaintext,(unsigned char*)(openssl_ciphertext.raw_data()),&aes_key);
        PP_QQ_LOG_D("nettle ciphertext: %s",HexEncode<ExtendedStdString>(openssl_ciphertext.const_raw_data(),openssl_ciphertext.size()).cdata());
    }

    return 0;
}

