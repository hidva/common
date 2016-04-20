#include "extended_std_string.h"
#include "gtest/gtest.h"

TEST(ExtendedStdStringTest,test1)
{
    {
        ExtendedStdString str;
        EXPECT_TRUE(str.empty());
        EXPECT_EQ(0,str.size());
    }

    {
        const char *str = "HelloWorld";
        const size_t kStrSize = strlen(str);

        ExtendedStdString str1 {str};
        EXPECT_EQ(kStrSize,str1.size());
        EXPECT_STREQ(str,str1.c_str());
    }

    {
        const char *str = "";
        const size_t kStrSize = strlen(str);

        ExtendedStdString str1 {str};
        EXPECT_EQ(kStrSize,str1.size());
        EXPECT_STREQ(str,str1.c_str());
    }

    {
        const char *str = "HelloWorld";
        const size_t kStrSize = strlen(str);

        ExtendedStdString str1 {str};
        ExtendedStdString str2 {str1};

        EXPECT_EQ(kStrSize,str2.size());
        EXPECT_STREQ(str,str2.c_str());
    }

    {
        const char *str = "";
        const size_t kStrSize = strlen(str);

        ExtendedStdString str1 {str};
        ExtendedStdString str2 {str1};

        EXPECT_EQ(kStrSize,str2.size());
        EXPECT_STREQ(str,str2.c_str());
    }

    {
        const char *str = "";
        const size_t kStrSize = strlen(str);

        ExtendedStdString str1 {str};
        ExtendedStdString str2 {std::move(str1)};

        EXPECT_TRUE(str1.empty());
        EXPECT_EQ(kStrSize,str2.size());
        EXPECT_STREQ(str,str2.c_str());
    }

    {
        const char *str = "HelloWorld";
        const size_t kStrSize = strlen(str);

        ExtendedStdString str1 {str};
        ExtendedStdString str2 {std::move(str1)};

        EXPECT_TRUE(str1.empty());
        EXPECT_EQ(kStrSize,str2.size());
        EXPECT_STREQ(str,str2.c_str());
    }

}


