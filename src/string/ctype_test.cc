#include "string/ctype.h"

#include <gtest/gtest.h>

TEST(CTypeTest, ToUpperTest) {
    EXPECT_EQ('A', ToUpper('a'));
    EXPECT_EQ('B', ToUpper('b'));
    EXPECT_EQ('C', ToUpper('c'));
    EXPECT_EQ('D', ToUpper('d'));
    EXPECT_EQ('E', ToUpper('e'));

    EXPECT_EQ('A', ToUpper('A'));
    EXPECT_EQ('B', ToUpper('B'));
    EXPECT_EQ('C', ToUpper('C'));
    EXPECT_EQ('D', ToUpper('D'));
    EXPECT_EQ('E', ToUpper('E'));

    EXPECT_EQ(1, ToUpper(1));
    EXPECT_EQ(2, ToUpper(2));
    EXPECT_EQ(3, ToUpper(3));
    EXPECT_EQ(4, ToUpper(4));
    EXPECT_EQ(5, ToUpper(5));
}

TEST(CTypeTest, ToLowerTest) {
    EXPECT_EQ('a', ToLower('a'));
    EXPECT_EQ('b', ToLower('b'));
    EXPECT_EQ('c', ToLower('c'));
    EXPECT_EQ('d', ToLower('d'));
    EXPECT_EQ('e', ToLower('e'));

    EXPECT_EQ('a', ToLower('A'));
    EXPECT_EQ('b', ToLower('B'));
    EXPECT_EQ('c', ToLower('C'));
    EXPECT_EQ('d', ToLower('D'));
    EXPECT_EQ('e', ToLower('E'));

    EXPECT_EQ(1, ToLower(1));
    EXPECT_EQ(2, ToLower(2));
    EXPECT_EQ(3, ToLower(3));
    EXPECT_EQ(4, ToLower(4));
    EXPECT_EQ(5, ToLower(5));
}

