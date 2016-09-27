
#include "glibc_cxx_wrap/13_low_level_io.h"
#include "glibc_cxx_wrap/14_file_system_interface.h"
#include "gtest/gtest.h"
#include "log/pp_qq_log.h"
#include "common/config.h"
#include "string/extended_std_string.h"

#include <signal.h>


namespace {

/* 耗空熵池,当该函数返回时,熵池是空的.
 */
void EatAllEntropy()
{
    int fd = cxx_open("/dev/random",O_RDONLY | O_NONBLOCK);
    ON_SCOPE_EXIT (closefd) {
        close(fd);
    };

    char buf[128];
    ssize_t rc = 0;
    while((rc = read(fd,buf,sizeof(buf))) != -1) {
        ;
    }

    if (errno != EAGAIN)
        THROW(errno,"read");
    return ;
}

void OnSig(int)
{

}

}

TEST(ReadNTest,test1)
{
    /* 本次对 ReadN() 的测试主要是依赖于'/dev/random'.
     * 因为对'/dev/random'的读取可能不会一次就读取完成.
     */

    int fd = cxx_open("/dev/random",O_RDONLY);
    ON_SCOPE_EXIT (close_fd) {
        close(fd);
    };

    EatAllEntropy();

    auto old_handler = signal(SIGINT,OnSig);
    ON_SCOPE_EXIT(recover_signal_handler) {
        signal(SIGINT,old_handler);
    };

    char buf[16];
    PP_QQ_LOG_I("开始测试 ReadN()这里你可以输入 CTRL+C 来生成一个中断信号,从而测试");
    PP_QQ_LOG_I("ReadN() 能否应付好中断");
    EXPECT_EQ(sizeof(buf),ReadN(fd,buf,sizeof(buf)));
}

#if defined(PP_QQ_COMMON_ON_ANDROID)
const char *test_dir_path = "/sdcard";
#else
const char *test_dir_path = "/tmp";
#endif
constexpr size_t kTestCap = 512UL * 1024UL * 1024UL;

TEST(WriteNTest,test1)
{
    ExtendedStdString expected_content(kTestCap,'x');

    // 确保当 WriteN() 内部对 expected_content.const_raw_data() 的访问超界时,会触发 sigsegv.
    // 如果不掉用该接口,则根据 std::string 的实现,可能会多分配空间,从而超界时可能不会 SIGSEGV.
    expected_content.shrink_to_fit();

    int fd = cxx_open(test_dir_path,O_RDWR | O_TMPFILE | O_EXCL,0666);
    ON_SCOPE_EXIT (close_fd) {
        close(fd);
    };

    WriteN(fd,expected_content.const_raw_data(),expected_content.size());

    struct stat filestat;
    cxx_fstat(fd,&filestat);
    EXPECT_EQ(kTestCap,static_cast<size_t>(filestat.st_size));

    cxx_lseek(fd,0,SEEK_SET);

    ExtendedStdString actual_content(kTestCap,'c');
    EXPECT_EQ(kTestCap,ReadN(fd,actual_content.raw_data(),actual_content.size()));

    EXPECT_EQ(expected_content,actual_content);
}

TEST(PreadNTest,test)
{
    ExtendedStdString expected_content(kTestCap,'x');
    for (size_t i = 0; i < kTestCap; ++i)
        expected_content[i] = static_cast<unsigned char>(i);
    PP_QQ_LOG_I("Now Begin");

    int fd = cxx_open(test_dir_path,O_RDWR | O_TMPFILE | O_EXCL,0666);
    ON_SCOPE_EXIT (closefd) {
        close(fd);
    };

    WriteN(fd,expected_content.const_raw_data(),expected_content.size());

    for (size_t i = 0; i < 64U; ++i) {
        ExtendedStdString actual_content(kTestCap,'x');
        auto rc = PreadN(fd,actual_content.raw_data(),actual_content.size(),i);
        EXPECT_EQ(kTestCap - i,rc);
        EXPECT_EQ(0,memcmp(expected_content.const_raw_data() + i,actual_content.const_raw_data(),rc));
    }
}

TEST(PwriteNTest,test)
{
    ExtendedStdString expected_content(kTestCap,'x');
    for (size_t i = 0; i < kTestCap; ++i)
        expected_content[i] = static_cast<unsigned char>(i);
    PP_QQ_LOG_I("Now Begin");

    for (size_t i = 0; i < 32U; ++i) {
        int fd = cxx_open(test_dir_path,O_RDWR | O_TMPFILE | O_EXCL,0666);
        ON_SCOPE_EXIT (closefd) {
            close(fd);
        };

        PwriteN(fd,expected_content.const_raw_data(),expected_content.size(),i);
        ExtendedStdString actual_content(kTestCap,'x');
        EXPECT_EQ(actual_content.size(),PreadN(fd,actual_content.raw_data(),actual_content.size(),i));
        EXPECT_EQ(expected_content,actual_content);
    }
}


