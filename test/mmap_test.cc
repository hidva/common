
#include <signal.h>

#include "glibc_cxx_wrap/13_low_level_io.h"
#include "glibc_cxx_wrap/14_file_system_interface.h"
#include "string/extended_std_string.h"
#include "log/pp_qq_log.h"

#if 0
// 演示一下 mmap(MAP_PRIVATE) 之后再修改文件内容,是否会对进程可见.
constexpr size_t k2GB = 2UL * 1024UL * 1024UL * 1024UL;
constexpr size_t k2GBPagesNumber = k2GB / 4096;

void on_sigint(int)
{
}

int
main(int argc,char **argv)
{
    signal(SIGINT,on_sigint);

    if (argc < 2) {
        printf("Usage: %s filename\n",argv[0]);
        return 1;
    }

    PP_QQ_LOG_I("2GB: %zu",k2GB);
    PP_QQ_LOG_I("2GBPagesNumber: %zu",k2GBPagesNumber);

    {
        int fd = cxx_open(argv[1],O_WRONLY | O_CREAT | O_EXCL,0666);
        ON_SCOPE_EXIT (closefd) {
            close(fd);
        };

        constexpr size_t k1GB = k2GB / 2;
        ExtendedStdString str(k1GB,'c');
        PP_QQ_CHECK(cxx_write(fd,str.const_raw_data(),str.size()) == k1GB);
        PP_QQ_CHECK(cxx_write(fd,str.const_raw_data(),str.size()) == k1GB);
    }

    int fd = cxx_open(argv[1],O_RDONLY);
    ON_SCOPE_EXIT (closefd) {
        close(fd);
    };

    unsigned char *mmap_ret = static_cast<unsigned char*>(cxx_mmap(nullptr,k2GB,PROT_READ,MAP_PRIVATE,fd,0));

    for (size_t i = 0;i < 33; ++i)
        PP_QQ_CHECK(mmap_ret[i] == 'c');

    pause();

    write(1,mmap_ret,33);
    write(1,"\n",1);

    unsigned char *last_page_ptr = mmap_ret + (k2GBPagesNumber - 1) * 4096;
    write(1,last_page_ptr,33);
    write(1,"\n",1);
    return 0;
}
#endif

#if 0
// 演示 MAP_FIXED 会覆盖已经存在的页
int
main(int argc,char **argv)
{
    void *mmap1_ret = cxx_mmap(nullptr,4096 * 2,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS,-1,0);
    memset(mmap1_ret,'c',4096 * 2);

    void *mmap2_ret = static_cast<unsigned char*>(mmap1_ret) - 4096;
    PP_QQ_CHECK((reinterpret_cast<unsigned long>(mmap2_ret) % 4096) == 0);

    size_t idx = 0;
    for (unsigned char *ptr = static_cast<unsigned char*>(mmap1_ret); idx < 4096; ++idx)
        PP_QQ_CHECK(ptr[idx] == 'c');
    PP_QQ_CHECK(cxx_mmap(mmap2_ret,4096 * 2,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,-1,0) == mmap2_ret);
    idx = 0;
    for (unsigned char *ptr = static_cast<unsigned char*>(mmap1_ret); idx < 4096; ++idx)
        PP_QQ_CHECK(ptr[idx] == 0);
    PP_QQ_LOG_D("DONE!");
    return 0;
}
#endif

#if 0
// 演示 mmap() 创建的 mapping 在 fork() 之后保留.
int
main(int argc,char **argv)
{
    PP_QQ_CHECK(argc >= 2);

    {
        int fd = cxx_open(argv[1],O_WRONLY | O_CREAT | O_EXCL,0666);
        ON_SCOPE_EXIT (closefd) {
            close(fd);
        };

        ExtendedStdString str(4096,'c');
        PP_QQ_CHECK(cxx_write(fd,str.const_raw_data(),str.size()) == str.size());
    }

    int fd = cxx_open(argv[1],O_RDWR);
    ON_SCOPE_EXIT (closefd) {
        close(fd);
    };

    unsigned char *mmap_ret = static_cast<unsigned char*>(cxx_mmap(nullptr,4096,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0));

    PP_QQ_LOG_I("In Parent!");
    PP_QQ_LOG_I("mmap_ret[0]: %c",mmap_ret[0]);

    pid_t fork_ret = fork();
    if (fork_ret == 0) {
        PP_QQ_LOG_I("In Child!");
        PP_QQ_LOG_I("mmap_ret[0]: %c",mmap_ret[0]);
        mmap_ret[0] = 'x';
        PP_QQ_LOG_I("mmap_ret[0]: %c",mmap_ret[0]);
        return 0;
    }

    sleep(3);
    PP_QQ_LOG_I("mmap_ret[0]: %c",mmap_ret[0]);
    return 0;
}
#endif

#if 0
// 演示 mmap() 创建的 mapping 在 fork() 之后保留.
int
main(int argc,char **argv)
{
    PP_QQ_CHECK(argc >= 2);

    {
        int fd = cxx_open(argv[1],O_WRONLY | O_CREAT | O_EXCL,0666);
        ON_SCOPE_EXIT (closefd) {
            close(fd);
        };

        ExtendedStdString str(4096,'c');
        PP_QQ_CHECK(cxx_write(fd,str.const_raw_data(),str.size()) == str.size());
    }

    int fd = cxx_open(argv[1],O_RDWR);
    ON_SCOPE_EXIT (closefd) {
        close(fd);
    };

    unsigned char *mmap_ret = static_cast<unsigned char*>(cxx_mmap(nullptr,4096,PROT_READ | PROT_WRITE,MAP_PRIVATE,fd,0));

    PP_QQ_LOG_I("In Parent!");
    PP_QQ_LOG_I("mmap_ret[0]: %c",mmap_ret[0]);

    pid_t fork_ret = fork();
    if (fork_ret == 0) {
        PP_QQ_LOG_I("In Child!");
        PP_QQ_LOG_I("mmap_ret[0]: %c",mmap_ret[0]);
        mmap_ret[0] = 'x';
        PP_QQ_LOG_I("mmap_ret[0]: %c",mmap_ret[0]);
        return 0;
    }

    sleep(3);
    PP_QQ_LOG_I("mmap_ret[0]: %c",mmap_ret[0]);
    return 0;
}
#endif

int
main(int argc,char **argv)
{
    unsigned char *mmap_ret = static_cast<unsigned char*>(cxx_mmap(nullptr,4096,PROT_READ | PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS,-1,0));
    memset(mmap_ret,'c',4096);

    PP_QQ_LOG_I("In Parent!");
    PP_QQ_LOG_I("mmap_ret[0]: %c",mmap_ret[0]);

    pid_t fork_ret = fork();
    if (fork_ret == 0) {
        PP_QQ_LOG_I("In Child!");
        PP_QQ_LOG_I("mmap_ret[0]: %c",mmap_ret[0]);
        mmap_ret[0] = 'x';
        PP_QQ_LOG_I("mmap_ret[0]: %c",mmap_ret[0]);
        return 0;
    }

    sleep(3);
    PP_QQ_LOG_I("mmap_ret[0]: %c",mmap_ret[0]);
    return 0;
}
