
#include "inlines.h"
#include "gtest/gtest.h"

TEST(IsEvenTest,test1)
{
    EXPECT_TRUE(IsEven(0));
    EXPECT_FALSE(IsEven(1));

    for (unsigned int i = 0; i <= 100; i += 2)
        EXPECT_TRUE(IsEven(i));
    for (unsigned int i = 3; i <= 107; i += 2)
        EXPECT_FALSE(IsEven(i));
}

TEST(IsOddTest,test1)
{
    for (unsigned int i = 0; i <= 100; i += 2)
        EXPECT_FALSE(IsOdd(i));
    for (unsigned int i = 3; i <= 107; i += 2)
        EXPECT_TRUE(IsOdd(i));
}

