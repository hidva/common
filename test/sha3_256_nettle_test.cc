#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "nettle/sha3.h"
#include "string/extended_std_string.h"
#include "hex_coding/hex_coding.h"
#include "exception/errno_exception.h"

int
main(int ,char **)
{
    int fd = open("/home/pp-qq/tmp/sha3_256.pc.txt",O_WRONLY | O_CREAT | O_APPEND | O_TRUNC,0666);
    PP_QQ_CHECK(fd >= 0);

    ON_SCOPE_EXIT (closefd) {
        close(fd);
    };

    uint8_t digest_buf[SHA3_256_DIGEST_SIZE];
    for (int i = 0; i <= 2048; ++i) {
        ExtendedStdString msg(i,'x');

        sha3_256_ctx sha3256ctx_;
        sha3_256_init(&sha3256ctx_);
        sha3_256_update(&sha3256ctx_,i,reinterpret_cast<const uint8_t*>(msg.const_raw_data()));
        sha3_256_digest(&sha3256ctx_,sizeof(digest_buf),digest_buf);

        ExtendedStdString digest_hex_str = HexEncode<ExtendedStdString>(digest_buf,SHA3_256_DIGEST_SIZE);
        digest_hex_str += '\n';
        int w = write(fd,digest_hex_str.const_raw_data(),digest_hex_str.size());
        PP_QQ_CHECK(w == digest_hex_str.size());
    }

    return 0;
}

