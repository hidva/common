
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log/pp_qq_log.h"
#include "exception/errno_exception.h"
#include "glibc_cxx_wrap/13_low_level_io.h"
#include "glibc_cxx_wrap/14_file_system_interface.h"
#include "rrid/scope_exit.h"

int
main(int argc,char **argv)
{
    if (argc < 4) {
        printf("Usage: %s filename offset content\n",argv[0]);
        printf("将会将 content 写入到 filename 指定偏移 offset 处\n");
        return 1;
    }

    const char *filename = argv[1];
    const char *content = argv[3];
    size_t content_size = strlen(content);
    int offset = atoi(argv[2]);

    PP_QQ_LOG_I("filename: '%s'",filename);
    PP_QQ_LOG_I("offset: %d",offset);
    PP_QQ_LOG_I("content: '%s'",content);

    int fd = cxx_open(filename,O_WRONLY | O_CREAT,0666);
    ON_SCOPE_EXIT (close_fd) {
        close(fd);
    };

    size_t rc = cxx_pwrite(fd,content,content_size,offset);
    PP_QQ_LOG_I("write bytes number: %zu",rc);

    if (rc != content_size)
        PP_QQ_LOG_W("content 未完全写入;content_size: %zu",content_size);

    return 0;
}

