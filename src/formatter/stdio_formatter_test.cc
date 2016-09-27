
#include "formatter/stdio_formatter.h"
#include "gtest/gtest.h"
#include <string>

TEST(AppendStdioFormatTest,test1)
{
    {
        std::string str;
        AppendStdioFormat(str,nullptr);
        EXPECT_EQ(std::string{},str);
        AppendStdioFormat(str,"");
        EXPECT_EQ(std::string{},str);
    }

    {
        const std::string kOriginStr{};
        std::string str = kOriginStr;
        AppendStdioFormat(str,nullptr);
        EXPECT_EQ(kOriginStr,str);
        AppendStdioFormat(str,"");
        EXPECT_EQ(kOriginStr,str);
    }

    {
        const std::string kOriginStr{"HelloWorld"};
        std::string str = kOriginStr;
        AppendStdioFormat(str,nullptr);
        EXPECT_EQ(kOriginStr,str);
        AppendStdioFormat(str,"");
        EXPECT_EQ(kOriginStr,str);
    }

    {
        std::string str{};
        AppendStdioFormat(str,"HelloWorld");
        EXPECT_EQ(std::string{"HelloWorld"},str);
    }

    {
        const std::string kOriginString{"HelloWorld"};
        EXPECT_EQ(10U, kOriginString.size());
        EXPECT_STREQ("HelloWorld",kOriginString.data());

        std::string str = kOriginString;
        AppendStdioFormat(str,"HelloWorld");
        EXPECT_EQ(std::string{"HelloWorldHelloWorld"},str);

        EXPECT_EQ(10U,kOriginString.size());
        EXPECT_STREQ("HelloWorld",kOriginString.data());
    }

    {
        std::string str{};
        AppendStdioFormat(str,"Hello: %d;%s;%c",10,"world",'c');
        EXPECT_EQ(std::string{"Hello: 10;world;c"},str);
    }

    {
        const std::string kOriginString{"HelloWorld"};
        EXPECT_EQ(10U,kOriginString.size());
        EXPECT_STREQ("HelloWorld",kOriginString.data());

        std::string str = kOriginString;
        AppendStdioFormat(str,"Hello: %d;%s;%c",10,"world",'c');
        EXPECT_EQ(std::string{"HelloWorldHello: 10;world;c"},str);

        EXPECT_EQ(10U,kOriginString.size());
        EXPECT_STREQ("HelloWorld",kOriginString.data());
    }

    {
        // 测试 __attribute__((format)) 有没有起作用.
#if 0
        std::string str;
        long tmp1;
        AppendStdioFormat(str,"hello %kf");
        AppendStdioFormat(str,"hello %d;%d",tmp1,tmp1);
#endif
    }
}

TEST(CreateStringFromStdioFormatTest,test1)
{
    EXPECT_EQ(std::string{},CreateStringFromStdioFormat<std::string>());
    EXPECT_EQ(std::string{},CreateStringFromStdioFormat<std::string>(static_cast<const char*>(nullptr)));
    EXPECT_EQ(std::string{},CreateStringFromStdioFormat<std::string>(""));
    EXPECT_EQ(std::string{"Hello"},CreateStringFromStdioFormat<std::string>("Hello"));
    EXPECT_EQ(std::string{"Hello: 33,world"},CreateStringFromStdioFormat<std::string>("Hello: %d,%s",33,"world"));
}

