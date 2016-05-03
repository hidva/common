
#include <openssl/sha.h>
#include "hex_coding/hex_coding.h"
#include "string/extended_std_string.h"
#include "exception/errno_exception.h"

int
main(int ,char **)
{
    unsigned char digest_buffer[SHA256_DIGEST_LENGTH];
    for (int i = 0; i <= 4096; ++i) {
        ExtendedStdString str(i,'x');


        SHA256_CTX sha256_ctx;
        PP_QQ_CHECK(SHA256_Init(&sha256_ctx) == 1);
        PP_QQ_CHECK(SHA256_Update(&sha256_ctx,str.const_raw_data(),str.size()) == 1);
        PP_QQ_CHECK(SHA256_Final(digest_buffer,&sha256_ctx) == 1);
        printf("\"%s\",\n",HexEncode<ExtendedStdString>(digest_buffer,SHA256_DIGEST_LENGTH).cdata());
    }
    return 0;
}



