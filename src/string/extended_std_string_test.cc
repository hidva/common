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

TEST(ExtendedStdStringTest,OperatorPlus)
{
    {
        auto Test = [] (const char *lhs,const char *rhs) {
            std::string l_str(lhs);
            std::string r_str(rhs);
            std::string sum_str(l_str + r_str);

            ExtendedStdString l(lhs);
            ExtendedStdString r(rhs);
            ExtendedStdString sum(l + r);

            EXPECT_EQ(l_str,l);
            EXPECT_EQ(r_str,r);
            EXPECT_EQ(sum_str,sum);
        };

        Test("","");
        Test("","Hello");
        Test("Hello","");
        Test("Hello","World");
    }

    {
        auto Test = [] (const char *lhs,const char *rhs) {
            std::string l_str(lhs);
            std::string r_str(rhs);
            std::string sum_str(std::move(l_str) + r_str);

            ExtendedStdString l(lhs);
            ExtendedStdString r(rhs);
            ExtendedStdString sum(std::move(l) + r);

            EXPECT_EQ(l_str,l);
            EXPECT_EQ(r_str,r);
            EXPECT_TRUE(l.empty());
            EXPECT_EQ(sum_str,sum);
        };

        Test("","");
        Test("","Hello");
        Test("Hello","");
        Test("Hello","World");
    }

    {
        auto Test = [] (const char *lhs,const char *rhs) {
            std::string l_str(lhs);
            std::string r_str(rhs);
            std::string sum_str(std::move(l_str) + std::move(r_str));

            ExtendedStdString l(lhs);
            ExtendedStdString r(rhs);
            ExtendedStdString sum(std::move(l) + std::move(r));

            EXPECT_EQ(l_str,l);
            EXPECT_EQ(r_str,r);
            EXPECT_TRUE(l.empty() || r.empty());
            EXPECT_EQ(sum_str,sum);
        };

        Test("","");
        Test("","Hello");
        Test("Hello","");
        Test("Hello","World");
    }

    {
        auto Test = [] (const char *lhs,const char *rhs) {
            std::string l_str(lhs);
            std::string r_str(rhs);
            std::string sum_str((l_str) + std::move(r_str));

            ExtendedStdString l(lhs);
            ExtendedStdString r(rhs);
            ExtendedStdString sum((l) + std::move(r));

            EXPECT_EQ(l_str,l);
            EXPECT_EQ(r_str,r);
            EXPECT_TRUE(r.empty());
            EXPECT_EQ(sum_str,sum);
        };

        Test("","");
        Test("","Hello");
        Test("Hello","");
        Test("Hello","World");
    }
}

TEST(ExtendedStdStringTest,OperatorPlus1)
{
    {
        auto Test = [] (const char *lstr,const char *rstr) {
            std::string rhs(rstr);
            std::string sum(lstr + rhs);

            ExtendedStdString r(rstr);
            ExtendedStdString s(lstr + r);

            EXPECT_EQ(rhs,r);
            EXPECT_EQ(sum,s);
        };

        Test("","");
        Test("","Hello");
        Test("Hello","");
        Test("Hello","World");
    }

    {
        auto Test = [] (const char *lstr,const char *rstr) {
            std::string rhs(rstr);
            std::string sum(lstr + std::move(rhs));

            ExtendedStdString r(rstr);
            ExtendedStdString s(lstr + std::move(r));

            EXPECT_EQ(rhs,r);
            EXPECT_EQ(sum,s);
            EXPECT_TRUE(r.empty());
        };

        Test("","");
        Test("","Hello");
        Test("Hello","");
        Test("Hello","World");
    }
}

TEST(ExtendedStdStringTest,OperatorPlus2)
{
    {
        auto Test = [] (const char *lstr,const char *rstr) {
            std::string rhs(rstr);
            std::string sum(lstr[0] + rhs);

            ExtendedStdString r(rstr);
            ExtendedStdString s(lstr[0] + r);

            EXPECT_EQ(rhs,r);
            EXPECT_EQ(sum,s);
        };

        Test("","");
        Test("","Hello");
        Test("Hello","");
        Test("Hello","World");
    }

    {
        auto Test = [] (const char *lstr,const char *rstr) {
            std::string rhs(rstr);
            std::string sum(lstr[0] + std::move(rhs));

            ExtendedStdString r(rstr);
            ExtendedStdString s(lstr[0] + std::move(r));

            EXPECT_EQ(rhs,r);
            EXPECT_EQ(sum,s);
            EXPECT_TRUE(r.empty());
        };

        Test("","");
        Test("","Hello");
        Test("Hello","");
        Test("Hello","World");
    }
}

TEST(ExtendedStdStringTest,OperatorPlus3)
{
    {
        auto Test = [] (const char *l,const char *r) {
            std::string l_str(l);
            std::string sum(l_str + r);

            ExtendedStdString l_estr(l);
            ExtendedStdString s_estr(l_estr + r);

            EXPECT_EQ(l_str,l_estr);
            EXPECT_EQ(sum,s_estr);
        };

        Test("","");
        Test("","Hello");
        Test("Hello","");
        Test("Hello","World");
    }

    {
        auto Test = [] (const char *l,const char *r) {
            std::string l_str(l);
            std::string sum(std::move(l_str) + r);

            ExtendedStdString l_estr(l);
            ExtendedStdString s_estr(std::move(l_estr) + r);

            EXPECT_EQ(l_str,l_estr);
            EXPECT_EQ(sum,s_estr);
            EXPECT_TRUE(l_estr.empty());
        };

        Test("","");
        Test("","Hello");
        Test("Hello","");
        Test("Hello","World");
    }
}

TEST(ExtendedStdStringTest,OperatorPlus4)
{
    {
        auto Test = [] (const char *l,const char *r) {
            std::string l_str(l);
            std::string sum(l_str + r[0]);

            ExtendedStdString l_estr(l);
            ExtendedStdString s_estr(l_estr + r[0]);

            EXPECT_EQ(l_str,l_estr);
            EXPECT_EQ(sum,s_estr);
        };

        Test("","");
        Test("","Hello");
        Test("Hello","");
        Test("Hello","World");
    }

    {
        auto Test = [] (const char *l,const char *r) {
            std::string l_str(l);
            std::string sum(std::move(l_str) + r[0]);

            ExtendedStdString l_estr(l);
            ExtendedStdString s_estr(std::move(l_estr) + r[0]);

            EXPECT_EQ(l_str,l_estr);
            EXPECT_EQ(sum,s_estr);
            EXPECT_TRUE(l_estr.empty());
        };

        Test("","");
        Test("","Hello");
        Test("Hello","");
        Test("Hello","World");
    }
}

TEST(ExtendedStdStringTest,Construct4)
{
    auto Test = [] (const char *src) {
        std::string str1(src);
        std::string str2(src);

        std::string str3(str1);
        ExtendedStdString str4(str2);

        EXPECT_EQ(str3,str4);
        EXPECT_EQ(str1,str2);
    };

    Test("");
    Test("H");
    Test("Hello");
}

TEST(ExtendedStdStringTest,Construct5)
{
    auto Test = [] (const char *src) {
        std::string str1(src);
        std::string str2(src);

        std::string str3(std::move(str1));
        ExtendedStdString str4(std::move(str2));

        EXPECT_EQ(str3,str4);
        EXPECT_EQ(str1,str2);
        EXPECT_TRUE(str1.empty());
        EXPECT_TRUE(str2.empty());
    };

    Test("");
    Test("H");
    Test("Hello");
}

TEST(ExtendedStdStringTest,OperatorEqual)
{
    auto Test = [] (const char *lsrc,const char *rsrc) {
        EXPECT_EQ(std::string(lsrc) == std::string(rsrc),ExtendedStdString(lsrc) == ExtendedStdString(rsrc));
        EXPECT_EQ(std::string(lsrc) == rsrc,ExtendedStdString(lsrc) == (rsrc));
        EXPECT_EQ((lsrc) == std::string(rsrc),(lsrc) == ExtendedStdString(rsrc));
    };

    Test("","");
    Test("","Hello");
    Test("Hello","");
    Test("Hello","World");
    Test("Hello","Hello");
    Test("World","Hello");
}

TEST(ExtendedStdStringTest,OperatorNotEqual)
{
    auto Test = [] (const char *lsrc,const char *rsrc) {
        EXPECT_EQ(std::string(lsrc) != std::string(rsrc),ExtendedStdString(lsrc) != ExtendedStdString(rsrc));
        EXPECT_EQ(std::string(lsrc) != rsrc,ExtendedStdString(lsrc) != (rsrc));
        EXPECT_EQ((lsrc) != std::string(rsrc),(lsrc) != ExtendedStdString(rsrc));
    };

    Test("","");
    Test("","Hello");
    Test("Hello","");
    Test("Hello","World");
    Test("Hello","Hello");
    Test("World","Hello");
}

TEST(ExtendedStdStringTest,OperatorLess)
{
    auto Test = [] (const char *lsrc,const char *rsrc) {
        EXPECT_EQ(std::string(lsrc) < std::string(rsrc),ExtendedStdString(lsrc) < ExtendedStdString(rsrc));
        EXPECT_EQ(std::string(lsrc) < rsrc,ExtendedStdString(lsrc) < (rsrc));
        EXPECT_EQ((lsrc) < std::string(rsrc),(lsrc) < ExtendedStdString(rsrc));
    };

    Test("","");
    Test("","Hello");
    Test("Hello","");
    Test("Hello","World");
    Test("Hello","Hello");
    Test("World","Hello");
}

TEST(ExtendedStdStringTest,OperatorGreater)
{
    auto Test = [] (const char *lsrc,const char *rsrc) {
        EXPECT_EQ(std::string(lsrc) > std::string(rsrc),ExtendedStdString(lsrc) > ExtendedStdString(rsrc));
        EXPECT_EQ(std::string(lsrc) > rsrc,ExtendedStdString(lsrc) > (rsrc));
        EXPECT_EQ((lsrc) > std::string(rsrc),(lsrc) > ExtendedStdString(rsrc));
    };

    Test("","");
    Test("","Hello");
    Test("Hello","");
    Test("Hello","World");
    Test("Hello","Hello");
    Test("World","Hello");
}

TEST(ExtendedStdStringTest,OperatorLessEqual)
{
    auto Test = [] (const char *lsrc,const char *rsrc) {
        EXPECT_EQ(std::string(lsrc) <= std::string(rsrc),ExtendedStdString(lsrc) <= ExtendedStdString(rsrc));
        EXPECT_EQ(std::string(lsrc) <= rsrc,ExtendedStdString(lsrc) <= (rsrc));
        EXPECT_EQ((lsrc) <= std::string(rsrc),(lsrc) <= ExtendedStdString(rsrc));
    };

    Test("","");
    Test("","Hello");
    Test("Hello","");
    Test("Hello","World");
    Test("Hello","Hello");
    Test("World","Hello");
}

TEST(ExtendedStdStringTest,OperatorGreaterEqual)
{
    auto Test = [] (const char *lsrc,const char *rsrc) {
        EXPECT_EQ(std::string(lsrc) >= std::string(rsrc),ExtendedStdString(lsrc) >= ExtendedStdString(rsrc));
        EXPECT_EQ(std::string(lsrc) >= rsrc,ExtendedStdString(lsrc) >= (rsrc));
        EXPECT_EQ((lsrc) >= std::string(rsrc),(lsrc) >= ExtendedStdString(rsrc));
    };

    Test("","");
    Test("","Hello");
    Test("Hello","");
    Test("Hello","World");
    Test("Hello","Hello");
    Test("World","Hello");
}

TEST(ExtendedStdStringTest,OperatorAssign)
{
    auto Test = [] (const char *dst,const char *src) {
        std::string dst_str(dst);
        std::string src_str(src);
        
        ExtendedStdString dst_ext_str(dst);
        ExtendedStdString src_ext_str(src);
        
        dst_str = src_str;
        dst_ext_str = src_ext_str;
        
        EXPECT_EQ(dst_str,dst_ext_str);
        EXPECT_EQ(src_str,src_ext_str);
    };
    
    Test("","");
    Test("","Hello");
    Test("Hello","");
    Test("Hello","World");
    Test("Hello","Hello");
    Test("World","Hello");
}

TEST(ExtendedStdStringTest,OperatorAssign1)
{
    auto Test = [] (const char *dst,const char *src) {
        std::string dst_str(dst);
        std::string src_str(src);
        
        ExtendedStdString dst_ext_str(dst);
        ExtendedStdString src_ext_str(src);
        
        dst_str = std::move(src_str);
        dst_ext_str = std::move(src_ext_str);
        
        EXPECT_EQ(dst_str,dst_ext_str);
        EXPECT_EQ(src_str,src_ext_str);
        EXPECT_EQ(src_str.empty(),src_ext_str.empty());
    };
    
    Test("","");
    Test("","Hello");
    Test("Hello","");
    Test("Hello","World");
    Test("Hello","Hello");
    Test("World","Hello");
}
