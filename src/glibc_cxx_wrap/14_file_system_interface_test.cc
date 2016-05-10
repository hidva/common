
#include "gtest/gtest.h"
#include "glibc_cxx_wrap/14_file_system_interface.h"
#include "string/extended_std_string.h"

TEST(AppendRealpathTest,test1)
{
    ExtendedStdString expected_val = GetCWD<ExtendedStdString>();
    EXPECT_EQ(expected_val,CreateStringFromRealPath<ExtendedStdString>(".///"));
}

TEST(AppendRealpathTest,test2)
{
    ExtendedStdString excepted_val("hello");
    GetCWD(excepted_val);

    ExtendedStdString actual_val("hello");
    AppendRealpath(actual_val,".");

    EXPECT_EQ(excepted_val,actual_val);
}

TEST(AppendRealpathTest,test3)
{
    auto Test = [] (const char *prefix) {
        const ExtendedStdString expect_val(prefix);
        ExtendedStdString actual_val(prefix);

        try {
            AppendRealpath(actual_val,"./_._this_is_not_exist_._");
            FAIL() << "当前目录下不应该存在名为'_._this_is_not_exist_._'的文件(或目录)!";
        } catch (const std::exception &e) {
            // 测试当异常发生时,actual_val 的状态不变
            EXPECT_EQ(expect_val,actual_val);
        }
    };

    Test("");
    Test("H");
    Test("Hello");
}


