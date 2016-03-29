
#define private public

#include "errno_exception.h"
#include "gtest/gtest.h"

TEST(ErrnoExceptionTest,test1)
{
    {
        auto exp1 = PP_QQ_CONSTRUCT_ERRNOEXCEPTION_OBJ(33);
        EXPECT_EQ(33,exp1.err_number_);
        EXPECT_EQ(std::string{},exp1.err_msg_);
    }

    {
        auto exp2 = PP_QQ_CONSTRUCT_ERRNOEXCEPTION_OBJ(33,"");
        EXPECT_EQ(33,exp2.err_number_);
        EXPECT_EQ(std::string{},exp2.err_msg_);
    }

    {
        auto exp3 = PP_QQ_CONSTRUCT_ERRNOEXCEPTION_OBJ(33,"hello");
        EXPECT_EQ(33,exp3.err_number_);
        EXPECT_EQ(std::string{"hello"},exp3.err_msg_);
    }

    {
        auto exp4 = PP_QQ_CONSTRUCT_ERRNOEXCEPTION_OBJ(33,"hello: %d;err: %s",33,"world");
        EXPECT_EQ(33,exp4.err_number_);
        EXPECT_EQ(std::string{"hello: 33;err: world"},exp4.err_msg_);
    }
}

