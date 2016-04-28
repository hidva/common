
#include "inline_utils.h"
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

TEST(IsOverlapTest,test1)
{
    EXPECT_FALSE(IsOverlap(nullptr,0,nullptr,0));
    EXPECT_FALSE(IsOverlap(nullptr,0,nullptr,1));
    EXPECT_FALSE(IsOverlap(nullptr,1,nullptr,0));
}

TEST(IsOverlapTest,test2)
{
    const unsigned char *base = reinterpret_cast<unsigned char*>(0x100UL);

    EXPECT_FALSE(IsOverlap(base + 0,7,base + 12,3));
    EXPECT_FALSE(IsOverlap(base + 1,7,base + 12,3));
    EXPECT_FALSE(IsOverlap(base + 2,7,base + 12,3));
    EXPECT_FALSE(IsOverlap(base + 3,7,base + 12,3));
    EXPECT_FALSE(IsOverlap(base + 4,7,base + 12,3));
    EXPECT_FALSE(IsOverlap(base + 5,7,base + 12,3));

    EXPECT_TRUE(IsOverlap(base + 6,7,base + 12,3));
    EXPECT_TRUE(IsOverlap(base + 7,7,base + 12,3));
    EXPECT_TRUE(IsOverlap(base + 8,7,base + 12,3));
    EXPECT_TRUE(IsOverlap(base + 9,7,base + 12,3));
    EXPECT_TRUE(IsOverlap(base + 10,7,base + 12,3));
    EXPECT_TRUE(IsOverlap(base + 11,7,base + 12,3));
    EXPECT_TRUE(IsOverlap(base + 12,7,base + 12,3));
    EXPECT_TRUE(IsOverlap(base + 13,7,base + 12,3));
    EXPECT_TRUE(IsOverlap(base + 14,7,base + 12,3));

    EXPECT_FALSE(IsOverlap(base + 15,7,base + 12,3));
    EXPECT_FALSE(IsOverlap(base + 16,7,base + 12,3));
    EXPECT_FALSE(IsOverlap(base + 17,7,base + 12,3));
}

TEST(IsOverlapTest,test3)
{
    const unsigned char *base = reinterpret_cast<unsigned char*>(0x100UL);

    EXPECT_FALSE(IsOverlap(base + 8,3,base + 12,3));
    EXPECT_FALSE(IsOverlap(base + 9,3,base + 12,3));

    EXPECT_TRUE(IsOverlap(base + 10,3,base + 12,3));
    EXPECT_TRUE(IsOverlap(base + 11,3,base + 12,3));
    EXPECT_TRUE(IsOverlap(base + 12,3,base + 12,3));
    EXPECT_TRUE(IsOverlap(base + 13,3,base + 12,3));
    EXPECT_TRUE(IsOverlap(base + 14,3,base + 12,3));

    EXPECT_FALSE(IsOverlap(base + 15,3,base + 12,3));
    EXPECT_FALSE(IsOverlap(base + 16,3,base + 12,3));
    EXPECT_FALSE(IsOverlap(base + 17,3,base + 12,3));
}

