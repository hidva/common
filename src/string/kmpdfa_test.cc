#include "string/kmpdfa.h"

#include <gtest/gtest.h>

TEST(KMPDFATest, CaseSensitive) {
    EXPECT_THROW(KMPDFA{""}, std::exception);

    KMPDFA kmpdfa;
    EXPECT_THROW(kmpdfa.Compile(""), std::exception);

    kmpdfa.Compile("a");
    EXPECT_EQ(3U, kmpdfa.Run("bbb"));
    EXPECT_EQ(1U, kmpdfa.Run("A"));
    EXPECT_EQ(3U, kmpdfa.Run("bcdaefg"));

    kmpdfa.Compile("abcd");
    EXPECT_EQ(2U, kmpdfa.Run("2b"));
    EXPECT_EQ(10U, kmpdfa.Run("ABCABCDEFG"));
    EXPECT_EQ(6U, kmpdfa.Run("bcdabcabcdefg"));

    kmpdfa.Compile("aaa");
    EXPECT_EQ(2U, kmpdfa.Run("2b"));
    EXPECT_EQ(9U, kmpdfa.Run("ABCAAACBA"));
    EXPECT_EQ(3U, kmpdfa.Run("abcaaacba"));
}

TEST(KMPDFATest, CaseInsensitive) {
    EXPECT_THROW(KMPDFA("", true), std::exception);

    KMPDFA kmpdfa;
    EXPECT_THROW(kmpdfa.Compile("", true), std::exception);

    kmpdfa.Compile("a", true);
    EXPECT_EQ(3U, kmpdfa.Run("bbb"));
    EXPECT_EQ(0U, kmpdfa.Run("A"));
    EXPECT_EQ(3U, kmpdfa.Run("bcdAefg"));

    kmpdfa.Compile("abcd", true);
    EXPECT_EQ(2U, kmpdfa.Run("2b"));
    EXPECT_EQ(3U, kmpdfa.Run("ABCABCDEFG"));
    EXPECT_EQ(6U, kmpdfa.Run("bcdabcabCDefg"));

    kmpdfa.Compile("aaa", true);
    EXPECT_EQ(2U, kmpdfa.Run("2b"));
    EXPECT_EQ(3U, kmpdfa.Run("ABCAAACBA"));
    EXPECT_EQ(3U, kmpdfa.Run("abcaAacba"));
}
