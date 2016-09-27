
#include "common/utils.h"
#include "gtest/gtest.h"

#include <string>
#include <vector>

TEST(MemCompareTest,test1) {
    EXPECT_EQ(0,MemCompare(nullptr,nullptr));
    EXPECT_EQ(0,MemCompare(nullptr,""));
    EXPECT_LT(MemCompare(nullptr,"a"),0);

    EXPECT_EQ(0,MemCompare("",nullptr));
    EXPECT_GT(MemCompare("a",nullptr),0);

    EXPECT_EQ(0,MemCompare("",""));
    EXPECT_LT(MemCompare("","A"),0);
    EXPECT_LT(0,MemCompare("A",""));

    EXPECT_EQ(0,MemCompare("0","0"));
    EXPECT_GT(0,MemCompare("0","1"));
    EXPECT_GT(0,MemCompare("0","00"));
    EXPECT_GT(0,MemCompare("00","01"));
    EXPECT_LT(0,MemCompare("10","01"));
    EXPECT_LT(0,MemCompare("1","0"));
    EXPECT_LT(0,MemCompare("00","0"));
}


TEST(LineTraverseTest,test)
{
    auto Split = [] (const char *src) -> std::vector<std::string> {
        std::vector<std::string> result;

        LineTraverse(src, [&] (const char *ptr, size_t size) -> LineTraverseReturn {
            ASSERT_NE(size, 0);
            result.emplace_back(ptr,size);
            return LineTraverseReturn::kContinue;
        });

        return result;
    };

    auto result = Split("");
    ASSERT_EQ(0,result.size());

    result = Split("\n\n\n");
    ASSERT_EQ(0,result.size());

    result = Split("hello");
    ASSERT_EQ(1,result.size());
    EXPECT_EQ("hello",result[0]);

    result = Split("hello\nworld");
    ASSERT_EQ(2,result.size());
    EXPECT_EQ("hello",result[0]);
    EXPECT_EQ("world",result[1]);

    result = Split("hello\nworld\n\n\n");
    ASSERT_EQ(2,result.size());
    EXPECT_EQ("hello",result[0]);
    EXPECT_EQ("world",result[1]);

    result = Split("\n\n\nhello\nworld");
    ASSERT_EQ(2,result.size());
    EXPECT_EQ("hello",result[0]);
    EXPECT_EQ("world",result[1]);

    result = Split("\n\n\nhello\nworld\n\n\n");
    ASSERT_EQ(2,result.size());
    EXPECT_EQ("hello",result[0]);
    EXPECT_EQ("world",result[1]);

    result = Split("\n\n\nhello\n\n\nworld\n\n\n");
    ASSERT_EQ(2,result.size());
    EXPECT_EQ("hello",result[0]);
    EXPECT_EQ("world",result[1]);
}

namespace {


}

TEST(GetUtf8LetterNumberTest, test) {
    EXPECT_EQ(0, GetUtf8LetterNumber(""));
    EXPECT_EQ(18, GetUtf8LetterNumber("Hello，文明富强。！·.,、【】"));
}

TEST(LoopbackTraverseTest,test) {
    auto Test = [] (const std::vector<int> &v,
        const std::vector<int>::const_iterator ptr,
        const std::vector<int> &expected) {
        std::vector<int> tmp;
        LoopbackTraverse(v.begin(), v.end(), ptr, [&] (int i) -> int {tmp.push_back(i); return 0; });
        EXPECT_EQ(expected, tmp);
    };

    std::vector<int> v1 {1};
    Test(v1, v1.begin(), {1});

    std::vector<int> v2 {1, 2};
    Test(v2, v2.begin(), {1, 2});
    Test(v2, v2.begin() + 1, {2, 1});

    std::vector<int> v4 {1, 2, 4, 8};
    Test(v4, v4.begin(), {1, 2, 4, 8});
    Test(v4, v4.begin() + 1, {2, 4, 8, 1});
    Test(v4, v4.begin() + 2, {4, 8, 1, 2});
    Test(v4, v4.begin() + 3, {8, 1, 2, 4});
}

TEST(GetSetIntersectionNumberTest, test) {
    auto DoTest = [] (const std::vector<int> &left, const std::vector<int> &right, size_t expect_val) {
        EXPECT_EQ(expect_val, GetSetIntersectionNumber(left.cbegin(), left.cend(), right.cbegin(), right.cend()));
    };

    DoTest({1, 2}, {4, 5, 6, 7, 8}, 0);
    DoTest({1, 2}, {1, 5, 6, 7, 8}, 1);
    DoTest({1, 2}, {-2, -1, 1, 2, 8}, 2);
}


