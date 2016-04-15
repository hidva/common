
#include "gtest/gtest.h"

#define private public

#include "errno_exception.h"

using namespace pp_qq;

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

/* 检测是否可以正常调用 ErrnoException 的移动构造函数.
 *
 * 如何检测是否调用了移动构造函数;根据 std::string 被移动构造后变为空串的常识.
 */
TEST(ErrnoExceptionTest,MoveTest)
{
    {
        // 首先检测 std::string 被移动构造后是否会变为空串,若不会,则检测无法继续!
        std::string str1 {"HelloWorld"};
        ASSERT_FALSE(str1.empty());
        std::string  {std::move(str1)};
        ASSERT_TRUE(str1.empty());
    }


    const std::string origin_function_name {__PRETTY_FUNCTION__};
    const std::string origin_err_msg {"HelloWorld"};

    {
        std::string function_name {origin_function_name};
        std::string err_msg {origin_err_msg};
        ErrnoException {function_name,EINVAL,err_msg};
        EXPECT_FALSE(function_name.empty());
        EXPECT_FALSE(err_msg.empty());
    }

    {
        std::string function_name {origin_function_name};
        std::string err_msg {origin_err_msg};
        ErrnoException {function_name,EINVAL,std::move(err_msg)};
        EXPECT_FALSE(function_name.empty());
        EXPECT_TRUE(err_msg.empty());
    }

    {
        std::string function_name {origin_function_name};
        std::string err_msg {origin_err_msg};
        ErrnoException {std::move(function_name),EINVAL,err_msg};
        EXPECT_TRUE(function_name.empty());
        EXPECT_FALSE(err_msg.empty());
    }

    {
        std::string function_name {origin_function_name};
        std::string err_msg {origin_err_msg};
        ErrnoException {std::move(function_name),EINVAL,std::move(err_msg)};
        EXPECT_TRUE(function_name.empty());
        EXPECT_TRUE(err_msg.empty());
    }

    {
        std::string function_name {origin_function_name};

        ErrnoException e{function_name,EINVAL,"hello"};
        EXPECT_EQ(function_name,e.function_name_);
        EXPECT_EQ(std::string{"hello"},e.err_msg_);

        ErrnoException e1{function_name,EINVAL,"hello: %d;%s",3,"world"};
        EXPECT_EQ(function_name,e1.function_name_);
        EXPECT_EQ(std::string{"hello: 3;world"},e1.err_msg_);
    }

    {
        std::string function_name {origin_function_name};

        ErrnoException e{std::move(function_name),EINVAL,"hello"};
        EXPECT_TRUE(function_name.empty());
        EXPECT_EQ(origin_function_name,e.function_name_);
        EXPECT_EQ(std::string{"hello"},e.err_msg_);

        function_name = origin_function_name;
        ErrnoException e1{std::move(function_name),EINVAL,"hello: %d;%s",3,"world"};
        EXPECT_TRUE(function_name.empty());
        EXPECT_EQ(origin_function_name,e.function_name_);
        EXPECT_EQ(std::string{"hello: 3;world"},e1.err_msg_);
    }

    {
        PP_QQ_CONSTRUCT_ERRNOEXCEPTION_OBJ();
        PP_QQ_CONSTRUCT_ERRNOEXCEPTION_OBJ(EINVAL);
        PP_QQ_CONSTRUCT_ERRNOEXCEPTION_OBJ(EINVAL,"err_msg_format");
        PP_QQ_CONSTRUCT_ERRNOEXCEPTION_OBJ(EINVAL,"err_msg_format: %d;%s",3,"world");
    }
}

TEST(ErrnoExceptionTest,CHECKMacroTest)
{
    PP_QQ_CHECK(true);
    PP_QQ_CHECK(true,EINVAL);
    PP_QQ_CHECK(true,EINVAL,"hello");
    PP_QQ_CHECK(true,EINVAL,"hello: %d",33);

    {
        try {
            PP_QQ_CHECK(false);
            FAIL() << "不应该执行这里!";
        } catch (const ErrnoException &e) {
            EXPECT_EQ(EINVAL,e.err_number_);
            EXPECT_TRUE(e.err_msg_.empty());
        }
    }

    {
        try {
            PP_QQ_CHECK(false,ENOTSUP);
            FAIL() << "不应该执行这里!";
        } catch (const ErrnoException &e) {
            EXPECT_EQ(ENOTSUP,e.err_number_);
            EXPECT_TRUE(e.err_msg_.empty());
        }
    }

    {
        try {
            PP_QQ_CHECK(false,ENOTSUP,"hello");
            FAIL() << "不应该执行这里!";
        } catch (const ErrnoException &e) {
            EXPECT_EQ(ENOTSUP,e.err_number_);
            EXPECT_EQ(std::string{"hello"},e.err_msg_);
        }
    }

    {
        try {
            PP_QQ_CHECK(false,ENOTSUP,"hello:%d;%s",33,"world");
            FAIL() << "不应该执行这里!";
        } catch (const ErrnoException &e) {
            EXPECT_EQ(ENOTSUP,e.err_number_);
            EXPECT_EQ(std::string{"hello:33;world"},e.err_msg_);
        }
    }
}

