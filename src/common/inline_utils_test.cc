
#include "inline_utils.h"
#include "gtest/gtest.h"

TEST(StrLenTest, test) {
    EXPECT_EQ((size_t)0, StrLen(nullptr));
    EXPECT_EQ((size_t)0, StrLen(""));

    const char *test_str = "SheHuiZhuYiJieBanRen";
    EXPECT_EQ((size_t)strlen(test_str), StrLen(test_str));
}

namespace {

bool init_done = false;

InitHelper g_init_1(
    [&] () noexcept {
        init_done = true;
        return ;
    };
);

} // namespace

TEST(InitHelperTest, test) {
    ASSERT_TRUE(init_done);
}

TEST(NormalizedTest, test) {
    auto DoTest = [] (long actual_sec, long actual_usec,
            long expected_sec, long expected_usec)  {
        struct timeval actual;
        actual.tv_sec = actual_sec;
        actual.tv_usec = actual_usec;
        Normalized(actual);
        EXPECT_EQ(expected_sec, actual.tv_sec);
        EXPECT_EQ(expected_usec, actual.tv_usec);
    };

    DoTest(3, 99999, 3, 99999);
    DoTest(3, 3000003, 6, 3);
}



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

