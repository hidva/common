
#include "string/acdfa.h"

#include <algorithm>

#include <gtest/gtest.h>

namespace {

using T = std::pair<size_t, size_t>;
using VT = std::vector<T>;

const VT Sort(VT src) {
    std::sort(src.begin(), src.end());
    return src;
}

} // namespace


TEST(ACDFATest, test1) {
    ACDFA acdfa;

    EXPECT_ANY_THROW(acdfa.Compile({"", "", ""}));

    acdfa.Compile({"He", "SHe", "Hem", "He"});
    EXPECT_EQ(Sort({{0, 3}, {1, 2}, {1, 2}, {1, 3}}), Sort(acdfa.Run("SHem")));
    EXPECT_EQ(Sort({}), Sort(acdfa.Run("hehe")));
    EXPECT_EQ(Sort({{2, 3}, {3, 2}, {3, 2}, {3, 3}}), Sort(acdfa.Run("heSHemhe")));

    acdfa.Compile({"He", "hE"});
    EXPECT_EQ(Sort({}), Sort(acdfa.Run("he")));
    EXPECT_EQ(Sort({{0, 2}, {4, 2}}), Sort(acdfa.Run("HehehE")));
    EXPECT_EQ(Sort({{2, 2}, {4, 2}}), Sort(acdfa.Run("heHehEhe")));

    acdfa.Compile({"HE", "she", "hEm", "hE"}, true);
    EXPECT_EQ(Sort({{0, 3}, {1, 2}, {1, 2}, {1, 3}}), Sort(acdfa.Run("SHEM")));
    EXPECT_EQ(Sort({{0, 2}, {2, 2}, {0, 2}, {2, 2}}), Sort(acdfa.Run("hehe")));
    EXPECT_EQ(Sort({}), Sort(acdfa.Run("abcd")));
}
