#include "string/string_piece.h"
#include "string/extended_std_string.h"
#include "log/pp_qq_log.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "gtest/gtest.h"

/*
 * 运行本文件内的测试用例需要 2180+ 秒,慎重.
 * <testsuite name="StringPieceTest" tests="61" failures="0" disabled="0" errors="0" time="2182.03">
 */

struct X {
    X()
    {
        srandom(time(nullptr));
    }
};

namespace {

X g_x_for_srandom;

}

/* 生成 [a,b] 中随机数.
 */
int RandomInt(int a,int b)
{
    int number = static_cast<int>(floor(random() / static_cast<double>(RAND_MAX) * (b - a) + a));
    return number < a ? (a) : ((number > b) ? b : number);
}

template <typename StringType>
StringType CreateRandomString(size_t size)
{
    StringType str;
    for (size_t idx = 0; idx < size; ++idx)
        str.push_back(RandomInt('a','z'));
    return std::move(str);
}

std::string CreateRandomStdString(size_t size)
{
    return CreateRandomString<std::string>(size);
}

TEST(StringPieceTest,ConstructTest1)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        StringPiece str(buf.data(),buf_size);
        ASSERT_EQ(0,str.size());
        ASSERT_TRUE(str.empty());
    }
}

TEST(StringPieceTest,ConstructTest2)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            std::string content = CreateRandomStdString(content_size);

            StringPiece str(buf.raw_data(),buf_size,content.data());
            ASSERT_EQ(content_size,str.size());
            ASSERT_EQ(0,memcmp(str.const_raw_data(),content.data(),content_size));
            ASSERT_EQ(buf.cdata(),str.const_raw_data());
        }
    }
}



TEST(StringPieceTest,ConstructTest3)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            std::string content = CreateRandomStdString(content_size);
            StringPiece str(buf.raw_data(),buf_size,content.data());

            StringPiece str1(std::move(str));
            ASSERT_TRUE(str.empty());
            ASSERT_EQ(content_size,str1.size());
            ASSERT_EQ(0,memcmp(str1.const_raw_data(),content.data(),content_size));
            ASSERT_EQ(buf.cdata(),str1.const_raw_data());
        }
    }
}

TEST(StringPieceTest,ConstructTest4)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            std::string content = CreateRandomStdString(content_size);

            StringPiece str(buf.raw_data(),buf_size,content.data(),content_size);
            ASSERT_EQ(content_size,str.size());
            ASSERT_EQ(0,memcmp(str.const_raw_data(),content.data(),content_size));
            ASSERT_EQ(buf.const_raw_data(),str.const_raw_data());
        }
    }
}

TEST(StringPieceTest,ConstructTest5)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf1(buf_size,'x');
        ExtendedStdString buf2(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size){
            const std::string content = CreateRandomStdString(content_size);
            const StringPiece str(buf1.data(),buf_size,content.data());

            for (size_t pos = 0; pos < str.size(); ++pos) {
                {
                    StringPiece str2(buf2.data(),buf_size,str,pos,str.npos);
                    ASSERT_EQ(buf2.const_raw_data(),str2.const_raw_data());
                    ASSERT_EQ(str.size() - pos,str2.size());
                    ASSERT_EQ(0,memcmp(str2.const_raw_data(),str.const_raw_data() + pos,str2.size()));
                }

                for (size_t len = 0; len <= str.size() - pos; ++len) {
                    //PP_QQ_LOG_D("buf_size: %zu;content_size: %zu;pos: %zu;len: %zu",buf_size,content_size,pos,len);
                    StringPiece str2(buf2.data(),buf_size,str,pos,len);
                    ASSERT_EQ(buf2.const_raw_data(),str2.const_raw_data());
                    ASSERT_EQ(len,str2.size());
                    ASSERT_EQ(0,memcmp(str2.const_raw_data(),str.const_raw_data() + pos,str2.size()));
                }
            }
        }
    }
}

TEST(StringPieceTest,Construct6)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            char ch = 'c';

            StringPiece str(buf.data(),buf_size,content_size,ch);
            ASSERT_EQ(buf.const_raw_data(),str.const_raw_data());
            ASSERT_EQ(content_size,str.size());
            for (size_t idx = 0; idx < str.size(); ++idx)
                ASSERT_EQ(ch,str.const_raw_data()[idx]);
        }
    }
}

TEST(StringPieceTest,Construct7)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            std::string content = CreateRandomStdString(content_size);

            StringPiece str(buf.raw_data(),buf_size,content.data(),content.data() + content_size);
            ASSERT_EQ(buf.const_raw_data(),str.const_raw_data());
            ASSERT_EQ(content_size,str.size());
            ASSERT_EQ(0,memcmp(str.const_raw_data(),content.data(),content_size));
        }
    }
}

inline bool
IsEqual(const std::string &lhs,const StringPiece &rhs)
{
    return rhs.size() == lhs.size() && (memcmp(rhs.const_raw_data(),lhs.data(),rhs.size()) == 0);
}

TEST(StringPieceTest,OperatorAssign1)
{
    for (size_t buf1_size = 0; buf1_size <= 100; ++buf1_size) {
        ExtendedStdString buf1(buf1_size,'x');

        for (size_t content1_size = 0; content1_size <= buf1_size; ++content1_size) {
            std::string content = CreateRandomStdString(content1_size);
            StringPiece str1(buf1.raw_data(),buf1_size,content.data(),content.size());

            for (size_t buf2_size = 0; buf2_size <= 100; ++buf2_size) {
                ExtendedStdString buf2(buf2_size,'x');

                for (size_t content2_size = 0; content2_size <= buf2_size; ++content2_size) {
                    StringPiece str2(buf2.raw_data(),buf2_size,content2_size,'x');

                    if (str1.size() > str2.capacity()) {
                        ASSERT_ANY_THROW(str2 = str1);
                    } else {
                        str2 = str1;
                        ASSERT_TRUE(IsEqual(content,str2));
                    }
                }
            }
        }
    }
}

TEST(StringPieceTest,OperatorAssgin2)
{
    for (size_t buf1_size = 0; buf1_size <= 100; ++buf1_size) {
        ExtendedStdString buf1(buf1_size,'x');

        for (size_t content1_size = 0; content1_size <= buf1_size; ++content1_size) {
            std::string content = CreateRandomStdString(content1_size);;
            StringPiece str1(buf1.raw_data(),buf1_size,content.data(),content.size());

            for (size_t buf2_size = 0; buf2_size <= 100; ++buf2_size) {
                ExtendedStdString buf2(buf2_size,'x');

                for (size_t content2_size = 0; content2_size <= buf2_size; ++content2_size) {
                    StringPiece str2(buf2.raw_data(),buf2_size,content2_size,'x');

                    if (str1.size() > str2.capacity()) {
                        ASSERT_ANY_THROW(str2 = std::move(str1));
                    } else {
                        str2 = std::move(str1);
                        ASSERT_TRUE(IsEqual(content,str2));
                    }
                }
            }
        }
    }
}

TEST(StringPieceTest,OperatorAssign3)
{
    for (size_t content_size = 0; content_size <= 100; ++content_size) {
        const std::string content = CreateRandomStdString(content_size);

        for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
            ExtendedStdString buf(buf_size,'x');
            StringPiece str(buf.raw_data(),buf_size);

            if (content.size() > str.capacity()) {
                ASSERT_ANY_THROW(str = content.data());
            } else {
                str = content.data();
                ASSERT_TRUE(IsEqual(content,str));
            }
        }
    }
}

TEST(StringPieceTest,OperatorAssign4)
{
    for (size_t buf_size = 0; buf_size <= 10; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');
        StringPiece str(buf.raw_data(),buf_size);
        char ch = 'c';

        if (str.capacity() < 1) {
            ASSERT_ANY_THROW(str = ch);
        } else {
            str = ch;
            ASSERT_EQ(1,str.size());
            ASSERT_EQ(ch,*str.const_raw_data());
        }
    }
}

TEST(StringPieceTest,BeginEndTest)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            const std::string content = CreateRandomStdString(content_size);
            StringPiece str(buf.raw_data(),buf_size,content.data(),content.size());

            ASSERT_EQ(str.size(),std::distance(str.begin(),str.end()));
            std::string content1(str.begin(),str.end());
            std::string content2;
            for (StringPiece::iterator iter = str.begin(); iter != str.end(); ++iter)
                content2.push_back(*iter);
            ASSERT_EQ(content,content1);
            ASSERT_EQ(content,content2);
        }
    }
}

TEST(StringPieceTest,BeginConstEndConst)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            const std::string content = CreateRandomStdString(content_size);
            const StringPiece str(buf.raw_data(),buf_size,content.data(),content.size());

            ASSERT_EQ(str.size(),std::distance(str.begin(),str.end()));
            std::string content1(str.begin(),str.end());
            std::string content2;
            for (StringPiece::const_iterator iter = str.begin(); iter != str.end(); ++iter)
                content2.push_back(*iter);
            ASSERT_EQ(content,content1);
            ASSERT_EQ(content,content2);
        }
    }
}


TEST(StringPieceTest,CBeginCEnd)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            const std::string content = CreateRandomStdString(content_size);
            StringPiece str(buf.raw_data(),buf_size,content.data(),content.size());

            ASSERT_EQ(str.size(),std::distance(str.cbegin(),str.cend()));
            std::string content1(str.cbegin(),str.cend());
            std::string content2;
            for (StringPiece::const_iterator iter = str.cbegin(); iter != str.cend(); ++iter)
                content2.push_back(*iter);
            ASSERT_EQ(content,content1);
            ASSERT_EQ(content,content2);
        }
    }
}

TEST(StringPieceTest,FrontBack)
{
    for (size_t buf_size = 1; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 1; content_size <= buf_size; ++content_size) {
            const std::string content = CreateRandomStdString(content_size);
            {
                StringPiece str(buf.raw_data(),buf_size,content.data(),content.size());
                ASSERT_EQ(content.front(),str.front());
                ASSERT_EQ(content.back(),str.back());
                ASSERT_EQ(content.front(),str.cfront());
                ASSERT_EQ(content.back(),str.cback());
            }
            {
                const StringPiece str(buf.raw_data(),buf_size,content.data(),content.size());
                ASSERT_EQ(content.front(),str.front());
                ASSERT_EQ(content.back(),str.back());
                ASSERT_EQ(content.front(),str.cfront());
                ASSERT_EQ(content.back(),str.cback());
            }
        }
    }
}

TEST(StringPieceTest,PopBack)
{
    for (size_t buf_size = 1; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 1; content_size <= buf_size; ++content_size) {
            std::string content = CreateRandomStdString(content_size);
            StringPiece str(buf.raw_data(),buf_size,content.data(),content.size());

            while (!content.empty()) {
                ASSERT_FALSE(str.empty());

                content.pop_back();
                str.pop_back();
                ASSERT_TRUE(IsEqual(content,str));
            }
            ASSERT_TRUE(str.empty());
        }
    }
}

TEST(StringPieceTest,PushBack)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            std::string content = CreateRandomStdString(content_size);
            StringPiece str(buf.raw_data(),buf_size,content.data(),content.size());

            while (str.size() < str.capacity()) {
                str.push_back('c');
                content.push_back('c');
                ASSERT_TRUE(IsEqual(content,str));
            }
            ASSERT_ANY_THROW(str.push_back('c'););
        }
    }
}

TEST(StringPieceTest,Capacity)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');
        StringPiece str(buf.raw_data(),buf_size);

        ASSERT_EQ(buf_size,str.capacity());
    }
}

TEST(StringPieceTest,Resize)
{
    for (size_t buf_size = 0;buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t new_size = 0; new_size <= buf_size; ++new_size) {
                std::string content = CreateRandomStdString(content_size);
                StringPiece str(buf.raw_data(),buf_size,content.data(),content.size());

                str.resize(new_size,'c');
                content.resize(new_size,'c');
                ASSERT_TRUE(IsEqual(content,str));
                ASSERT_ANY_THROW(str.resize(buf_size + new_size + 1,'x'));
            }
        }
    }
}

TEST(StringPieceTest,Reserve)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');
        StringPiece str(buf.raw_data(),buf_size);

        for (size_t new_capacity = 0; new_capacity <= buf_size + 3; ++new_capacity) {
            if (new_capacity > buf_size) {
                ASSERT_ANY_THROW(str.reserve(new_capacity));
            } else {
                str.reserve(new_capacity);
                ASSERT_GE(str.capacity(),new_capacity);
            }
        }
    }
}

TEST(StringPieceTest,Clear)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            StringPiece str(buf.raw_data(),buf_size,content_size,'c');

            str.clear();
            ASSERT_TRUE(str.empty());
        }
    }
}

TEST(StringPieceTest,OperatorBracketWrite)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            std::string content1 = CreateRandomStdString(content_size);;
            std::string content2 = CreateRandomStdString(content_size);;
            ASSERT_EQ(content2.size(),content1.size());
            StringPiece str(buf.raw_data(),buf_size,content1.data(),content1.size());

            for (size_t idx = 0; idx < str.size(); ++idx)
                str[idx] = content2[idx];
            ASSERT_TRUE(IsEqual(content2,str));
        }
    }
}

TEST(StringPieceTest,OperatorBracketReadOnly)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            const StringPiece str(buf.raw_data(),buf_size,content.data(),content.size());

            for (size_t idx = 0; idx < str.size(); ++idx)
                ASSERT_EQ(str[idx],content[idx]);
        }
    }
}

TEST(StringPieceTest,Empty)
{
    // empty() 的正确性也完全取决于 size(),所以也没有单独的测试用例.
}

TEST(StringPieceTest,ShiftToFit)
{
    // 该函数是个空函数,所以没有测试用例.
}

TEST(StringPieceTest,MaxSize)
{
    // max_size();朕不知道这个该怎么测试.
}

TEST(StringPieceTest,Length)
{
    // length() 完全等同于 size(),所以也没有单独的测试用例.
}

TEST(StringPieceTest,Size)
{
    // 对 size() 的测试分散个各个测试用例之间,所以并没有单独的测试用例来测试 size().
}


/* rbegin(),rbegin() const,crbegin(),rend(),rend() const,crend() 这几个
 * API 未测试,因为并不是很熟悉整个迭代器体系,所以没有贸然写出测试用例
 */

TEST(StringPieceTest,AtAndCat)
{
    // at() 的实现与 operator[] 完全一致,所以没有必要单独测试
    char buf[16];

    {
        StringPiece str(buf,sizeof(buf),"h");
        ASSERT_EQ('h',str.at(0));
        ASSERT_EQ('h',str.cat(0));
    }

    {
        const StringPiece str(buf,sizeof(buf),"h");
        ASSERT_EQ('h',str.at(0));
        ASSERT_EQ('h',str.cat(0));
    }
}

TEST(StringPieceTest,OperatorPlusEqual)
{
    // operator+= 直接调用了 append();所以这里没有测试.

    char buf[16];
    StringPiece str(buf,sizeof(buf));
    str += "H";
    ASSERT_TRUE(IsEqual(std::string("H"),str));
    str += 'c';
    ASSERT_TRUE(IsEqual(std::string("Hc"),str));

    char buf2[32];
    StringPiece str2(buf2,sizeof(buf2));
    str2 += str;
    ASSERT_TRUE(IsEqual(std::string("Hc"),str2));
}

/* 在 append() 系列中,仅测试了最底层的 2 个函数.
 * 以后有时间再增加对其他 append() 的测试用例吧
 */
TEST(StringPieceTest,Append1)
{
    {
        char buf1[100];
        char buf2[100];

        StringPiece str1(buf1,sizeof(buf1),"HelloWorld");
        StringPiece str2(buf2,sizeof(buf2));

        str2.append(str1);
        ASSERT_TRUE(IsEqual(std::string("HelloWorld"),str2));

        str2.append(str1,3,str1.npos);
        ASSERT_TRUE(IsEqual("HelloWorldloWorld",str2));

        str2.append(str1,3,2);
        ASSERT_TRUE(IsEqual("HelloWorldloWorldlo",str2));
    }

    {
        char buf1[100];
        char buf2[100];

        StringPiece str1(buf1,sizeof(buf1),"HelloWorld");
        StringPiece str2(buf2,sizeof(buf2));

        str2.append(std::move(str1));
        ASSERT_TRUE(IsEqual(std::string("HelloWorld"),str2));
    }

    {
        char buf[100];
        StringPiece str(buf,sizeof(buf));
        str.append("HelloWorld");
        ASSERT_TRUE(IsEqual("HelloWorld",str));
    }
}

TEST(StringPieceTest,Append2)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t append_size = 0; append_size <= buf_size - content_size; ++append_size) {
                std::string content (CreateRandomStdString(content_size));
                StringPiece str(buf.raw_data(),buf_size,content.data(),content.size());

                str.append(append_size,'x');
                content.append(append_size,'x');
                ASSERT_TRUE(IsEqual(content,str));

                ASSERT_ANY_THROW(str.append(str.capacity() - str.size() + append_size + 1,'x'));
            }
        }
    }
}

#define STRING_PIECE_TEST(item) TEST(StringPieceTest,item)

STRING_PIECE_TEST(Append3)
{
    // append(const value_type *s,size_type n);其中 [s,n) 不会与当前字符串相交.

    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t append_size = 0; append_size <= buf_size - content_size; ++append_size) {
                std::string content (CreateRandomStdString(content_size));
                StringPiece str(buf.raw_data(),buf_size,content.data(),content.size());

                std::string append_content (CreateRandomStdString(append_size));
                str.append(append_content.data(),append_content.size());
                content.append(append_content.data(),append_content.size());
                ASSERT_TRUE(IsEqual(content,str));

            }
        }
    }

    {
        char buf[16];
        StringPiece str(buf,sizeof(buf));
        char append_content[18];
        ASSERT_ANY_THROW(str.append(append_content,sizeof(append_content)));
    }
}

STRING_PIECE_TEST(Append4)
{
    // append(const value_type *s,size_type n);其中 [s,n) 与当前字符串相交.

    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buf(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t pos = 0; pos <= content_size; ++pos) {
                for (size_t len = 0; len <= content_size - pos; ++len) {
                    std::string content (CreateRandomStdString(content_size));
                    StringPiece str(buf.raw_data(),buf_size,content.data(),content.size());

                    if (len + str.size() > str.capacity()) {
                        ASSERT_ANY_THROW(str.append(str.const_raw_data() + pos,len));
                    } else {
                        content.append(str.const_raw_data() + pos,len);
                        str.append(str.const_raw_data() + pos,len);
                        ASSERT_TRUE(IsEqual(content,str));
                    }
                }
            }
        }
    }
}


STRING_PIECE_TEST(Assign1)
{
    // 在构造函数,operator=的测试用例中已经针对 assign() 的大多数情况进行测试.
    // 这里测试 assign(const StringPiece& str, const size_type pos,size_type n) 中 str 为 this 的情况

    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buffer(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t pos = 0; pos <= content_size; ++pos) {
                for (size_t len = 0; len <= content_size - pos; ++len) {
                    std::string content(CreateRandomStdString(content_size));
                    StringPiece str(buffer.raw_data(),buf_size,content.data(),content.size());

                    content.assign(str.const_raw_data() + pos,len);
                    str.assign(str,pos,len);
                    ASSERT_TRUE(IsEqual(content,str));
                }
            }
        }
    }
}

STRING_PIECE_TEST(Assign2)
{
    // 这里测试 assign(const value_type *s,const size_type n) 中 [s,n) 与当前字符串相交的情况.

    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buffer(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t pos = 0; pos <= content_size; ++pos) {
                for (size_t len = 0; len <= content_size - pos; ++len) {
                    std::string content(CreateRandomStdString(content_size));
                    StringPiece str(buffer.raw_data(),buf_size,content.data(),content.size());

                    content.assign(str.const_raw_data() + pos,len);
                    str.assign(str.const_raw_data() + pos,len);
                    ASSERT_TRUE(IsEqual(content,str));
                }
            }
        }
    }
}

STRING_PIECE_TEST(Insert)
{
    // insert(size_type pos,const value_type *s,size_type n) 其中 [s,n) 与当前字符串没有交集
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buffer(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t pos = 0; pos <= content_size; ++pos) {
                for (size_t n = 0; n <= buf_size - content_size; ++n) {
                    std::string content(CreateRandomStdString(content_size));
                    StringPiece str(buffer.raw_data(),buf_size,content.data(),content.size());

                    std::string insert_content(CreateRandomStdString(n));
                    content.insert(pos,insert_content.data(),n);
                    str.insert(pos,insert_content.data(),n);
                    ASSERT_TRUE(IsEqual(content,str));
                }
            }
        }
    }

    {
        char buf[16];
        StringPiece str(buf,sizeof(buf));
        char append_content[18];
        ASSERT_ANY_THROW(str.insert(0,append_content,sizeof(append_content)));
    }
}

STRING_PIECE_TEST(Insert1)
{
    // insert(size_type pos,const value_type *s,size_type n); 其中 [s,n) 与当前字符串有交集.
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buffer(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t pos = 0; pos <= content_size; ++pos) {
                for (size_t pos1 = 0; pos1 <= content_size; ++pos1) {
                    for (size_t len = 0; len <= content_size - pos1; ++len) {
                        std::string content(CreateRandomStdString(content_size));
                        StringPiece str(buffer.raw_data(),buf_size,content.data(),content.size());

                        if (str.size() + len > str.capacity()) {
                            ASSERT_ANY_THROW(str.insert(pos,str.const_raw_data() + pos1,len));
                        } else {
                            content.insert(pos,str.const_raw_data() + pos1,len);
                            str.insert(pos,str.const_raw_data() + pos1,len);
                            ASSERT_TRUE(IsEqual(content,str));
                        }
                    }
                }
            }
        }
    }
}

STRING_PIECE_TEST(Insert2)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buffer(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t pos = 0; pos <= content_size; ++pos) {
                for (size_t n = 0; n <= buf_size - content_size; ++n) {
                    std::string content(CreateRandomStdString(content_size));
                    StringPiece str(buffer.raw_data(),buf_size,content.data(),content.size());

                    content.insert(pos,n,'x');
                    str.insert(pos,n,'x');
                    ASSERT_TRUE(IsEqual(content,str));
                }
            }
        }
    }

    {
        char buf[16];
        StringPiece str(buf,sizeof(buf));
        ASSERT_ANY_THROW(str.insert(2,sizeof(buf) + 1,'x'));
    }
}

STRING_PIECE_TEST(Insert3)
{
    // 这里只需要简单测试以下函数接口就行,比较她们底层核心已经被全面测试了.
    // insert(size_type pos1, const StringPiece& str);
    // insert(size_type pos1, const StringPiece& str,size_type pos2, size_type n);
    // insert(size_type pos, const value_type* s);
    // insert(const_iterator p, const value_type c);
    // insert(const_iterator p, size_type n, value_type c);
    // 但是我现在写烦了咧,有空有空吧
}

STRING_PIECE_TEST(Erase1)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buffer(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t pos = 0; pos <= content_size; ++pos) {
                for (size_t n = 0; n <= content_size - pos; ++n) {
                    std::string content(CreateRandomStdString(content_size));
                    StringPiece str(buffer.raw_data(),buf_size,content.data(),content.size());

                    content.erase(pos,n);
                    str.erase(pos,n);
                    ASSERT_TRUE(IsEqual(content,str));
                }

                std::string content(CreateRandomStdString(content_size));
                StringPiece str(buffer.raw_data(),buf_size,content.data(),content.size());

                content.erase(pos);
                str.erase(pos);
                ASSERT_TRUE(IsEqual(content,str));
            }
        }
    }
}

STRING_PIECE_TEST(Erase2)
{
    // 这里只需要简单测试以下接口:
    // erase(const_iterator position);
    // erase(const_iterator first, const_iterator last);
}

STRING_PIECE_TEST(Replace2)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buffer(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t pos = 0; pos <= content_size; ++pos) {
                for (size_t n1 = 0; n1 <= content_size - pos; ++n1) {
                   for (size_t n2 = 0; n2 <= buf_size - (content_size - n1); ++n2) {
                       std::string content(CreateRandomStdString(content_size));
                       StringPiece str(buffer.raw_data(),buf_size,content.data(),content.size());

                       content.replace(pos,n1,n2,'x');
                       str.replace(pos,n1,n2,'x');
                       ASSERT_TRUE(IsEqual(content,str));
                   }
                }

                size_t n1 = content_size - pos;
                for (size_t n2 = 0; n2 <= buf_size - (content_size - n1); ++n2) {
                    std::string content(CreateRandomStdString(content_size));
                    StringPiece str(buffer.raw_data(),buf_size,content.data(),content.size());

                    content.replace(pos,std::string::npos,n2,'x');
                    str.replace(pos,StringPiece::npos,n2,'x');
                    ASSERT_TRUE(IsEqual(content,str));
                }
            }
        }
    }

    {

    }
}

STRING_PIECE_TEST(Replace3)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buffer(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t pos = 0; pos <= content_size; ++pos) {
                for (size_t n1 = 0; n1 <= content_size - pos; ++n1) {
                   for (size_t n2 = 0; n2 <= buf_size - (content_size - n1); ++n2) {
                       std::string content(CreateRandomStdString(content_size));
                       StringPiece str(buffer.raw_data(),buf_size,content.data(),content.size());

                       std::string replace_content(CreateRandomStdString(n2));
                       content.replace(pos,n1,replace_content.data(),replace_content.size());
                       str.replace(pos,n1,replace_content.data(),replace_content.size());
                       ASSERT_TRUE(IsEqual(content,str));
                   }
                }

                size_t n1 = content_size - pos;
                for (size_t n2 = 0; n2 <= buf_size - (content_size - n1); ++n2) {
                    std::string content(CreateRandomStdString(content_size));
                    StringPiece str(buffer.raw_data(),buf_size,content.data(),content.size());

                    std::string replace_content(CreateRandomStdString(n2));
                    content.replace(pos,std::string::npos,replace_content.data(),replace_content.size());
                    str.replace(pos,StringPiece::npos,replace_content.data(),replace_content.size());
                    ASSERT_TRUE(IsEqual(content,str));
                }
            }
        }
    }

    {

    }
}

STRING_PIECE_TEST(Replace4)
{

}

STRING_PIECE_TEST(Copy)
{
    for (size_t buf_size = 0; buf_size <= 100; ++buf_size) {
        ExtendedStdString buffer(buf_size,'x');

        for (size_t content_size = 0; content_size <= buf_size; ++content_size) {
            for (size_t pos = 0; pos <= content_size; ++pos) {
                for (size_t len = 0; len <= content_size - pos; ++len) {
                    const std::string content(CreateRandomStdString(content_size));
                    const StringPiece str(buffer.raw_data(),buf_size,content.data(),content.size());

                    ExtendedStdString buffer1(len,'x');
                    ExtendedStdString buffer2(len,'x');

                    content.copy(buffer1.raw_data(),len,pos);
                    str.copy(buffer2.raw_data(),len,pos);
                    ASSERT_EQ(0,memcmp(buffer1.const_raw_data(),buffer2.const_raw_data(),len));
                }
            }
        }
    }
}

STRING_PIECE_TEST(Swap)
{
    for (size_t buffer1_size = 0; buffer1_size <= 100; ++buffer1_size) {
        ExtendedStdString buffer1(buffer1_size,'x');
        for (size_t content1_size = 0; content1_size <= buffer1_size; ++content1_size) {
            std::string content1(CreateRandomStdString(content1_size));
            StringPiece string1(buffer1.raw_data(),buffer1_size,content1.data(),content1.size());

            for (size_t buffer2_size = 0; buffer2_size <= 100; ++buffer2_size) {
                ExtendedStdString buffer2(buffer2_size,'x');
                for (size_t content2_size = 0; content2_size <= buffer2_size; ++content2_size) {
                    std::string content2(CreateRandomStdString(content2_size));
                    StringPiece string2(buffer2.raw_data(),buffer2_size,content2.data(),content2.size());

                    if (string2.size() > string1.capacity() || string2.capacity() < string1.size()) {
                        ASSERT_ANY_THROW(string1.swap(string2));
                    } else {
                        content1.swap(content2);
                        string1.swap(string2);
                        ASSERT_TRUE(IsEqual(content1,string1));
                        ASSERT_TRUE(IsEqual(content2,string2));
                    }
                }
            }
        }
    }
}

STRING_PIECE_TEST(CStr)
{
    // 由于 c_str() 中直接调用了 data(),所以不再单独提供测试用例
}

STRING_PIECE_TEST(Data)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            if (content_size >= buffer_size) {
                ASSERT_ANY_THROW(str.data());
            } else {
                char *str_ptr = str.data();
                ASSERT_EQ('\0',str_ptr[content_size]);
                ASSERT_EQ(0,content.compare(0,content.npos,str_ptr,content_size));

                // 测试 data() 返回指针的可写性.
                const std::string content2(CreateRandomStdString(content_size));
                for (size_t idx = 0; idx < content_size; ++idx)
                    str_ptr[idx] = content2.at(idx);
                ASSERT_TRUE(IsEqual(content2,str));
            }
        }
    }
}


STRING_PIECE_TEST(Data1)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            const StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            if (content_size >= buffer_size) {
                ASSERT_ANY_THROW(str.data());
            } else {
                const char *str_ptr = str.data();
                ASSERT_EQ('\0',str_ptr[content_size]);
                ASSERT_EQ(0,content.compare(0,content.npos,str_ptr,content_size));
            }
        }
    }
}

STRING_PIECE_TEST(CData1)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            if (content_size >= buffer_size) {
                ASSERT_ANY_THROW(str.cdata());
            } else {
                const char *str_ptr = str.cdata();
                ASSERT_EQ('\0',str_ptr[content_size]);
                ASSERT_EQ(0,content.compare(0,content.npos,str_ptr,content_size));
            }
        }
    }
}

STRING_PIECE_TEST(RawData)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            char *str_ptr = str.raw_data();
            ASSERT_EQ(0,content.compare(0,content.npos,str_ptr,content_size));

            const std::string content2(CreateRandomStdString(content_size));
            for (size_t idx = 0; idx < content_size; ++idx)
                str_ptr[idx] = content2.at(idx);
            ASSERT_TRUE(IsEqual(content2,str));
        }
    }
}

STRING_PIECE_TEST(RawData1)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            const StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            const char *str_ptr = str.raw_data();
            ASSERT_EQ(0,content.compare(0,content.npos,str_ptr,content_size));
        }
    }
}

STRING_PIECE_TEST(RawData2)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            const char *str_ptr = str.const_raw_data();
            ASSERT_EQ(0,content.compare(0,content.npos,str_ptr,content_size));
        }
    }
}


/* 下面开始引入了 facebook/folly 中 FBStringTest.cpp 中使用的测试方法.
 *
 * 1. 基本思想
 * 给定值相同的 std::string,StringPiece 对象;然后对这两个对象做同样的操作,然后根据操作的返
 * 回值相同与否来判断 StringPiece 上的操作是否正确.
 *
 * 2. 如何新增测试用例
 * - 测试用例原型:
 *      template <typename StringType>
 *      auto TestCase(StringType &str);
 * - 测试框架对测试用例的使用:
 *      std::string string (CreateRandomStdString(content_size));
 *      StringPiece string2 (buffer,buffer_size,string.data(),string.size());
 *      ASSERT_EQ(string,string2);
 *
 *      // 确保在调用 TestCase<std::string>,TestCase<StringPiece> 时产生相同的随机数
 *      // 序列.
 *      srandom(1);
 *      auto expected = TestCase<std::string>(string);
 *      srandom(1);
 *      auto fact = TestCase<StringPiece>(string2);
 *      ASSERT_EQ(string,string2);
 *      ASSERT_EQ(expect,fact);
 */


void TestFind(const std::string &content,const StringPiece &str)
{
    ASSERT_TRUE(IsEqual(content,str));
    for (size_t pos1 = 0; pos1 <= content.size() + 3; ++pos1) {
        for (size_t pos2 = 0; pos2 <= content.size(); ++pos2) {
            for (size_t len = 0; len <= content.size() - pos2; ++len) {
                auto rc1 = content.find(str.const_raw_data() + pos2,pos1,len);
                int expected_rc = (rc1 == content.npos ? -1 : static_cast<int>(rc1));

                auto rc2 = str.find(str.const_raw_data() + pos2,pos1,len);
                int actual_rc = (rc2 == str.npos ? -1 : static_cast<int>(rc2));
                ASSERT_EQ(expected_rc,actual_rc);
            }
        }

        size_t kFindContentSizeMax = content.size() + content.size() / 2;
        for (size_t find_content_size = 0; find_content_size <= kFindContentSizeMax; ++find_content_size) {
            const std::string kFindContent(CreateRandomStdString(find_content_size));

            auto rc1 = content.find(kFindContent.data(),pos1,kFindContent.size());
            int expected_rc = (rc1 == content.npos ? -1 : static_cast<int>(rc1));

            auto rc2 = str.find(kFindContent.data(),pos1,kFindContent.size());
            int actual_rc = (rc2 == str.npos ? -1 : static_cast<int>(rc2));
            ASSERT_EQ(expected_rc,actual_rc);
        }
    }
}

void TestRFind(const std::string &content,const StringPiece &str)
{
    ASSERT_TRUE(IsEqual(content,str));
    for (size_t pos1 = 0; pos1 <= content.size() + 3; ++pos1) {
        for (size_t pos2 = 0; pos2 <= content.size(); ++pos2) {
            for (size_t len = 0; len <= content.size() - pos2; ++len) {
                auto rc1 = content.rfind(str.const_raw_data() + pos2,pos1,len);
                int expected_rc = (rc1 == content.npos ? -1 : static_cast<int>(rc1));

                auto rc2 = str.rfind(str.const_raw_data() + pos2,pos1,len);
                int actual_rc = (rc2 == str.npos ? -1 : static_cast<int>(rc2));
                ASSERT_EQ(expected_rc,actual_rc);
            }
        }

        size_t kFindContentSizeMax = content.size() + content.size() / 2;
        for (size_t find_content_size = 0; find_content_size <= kFindContentSizeMax; ++find_content_size) {
            const std::string kFindContent(CreateRandomStdString(find_content_size));

            auto rc1 = content.rfind(kFindContent.data(),pos1,kFindContent.size());
            int expected_rc = (rc1 == content.npos ? -1 : static_cast<int>(rc1));

            auto rc2 = str.rfind(kFindContent.data(),pos1,kFindContent.size());
            int actual_rc = (rc2 == str.npos ? -1 : static_cast<int>(rc2));
            ASSERT_EQ(expected_rc,actual_rc);
        }
    }
}

void TestFindFirstOf(const std::string &content,const StringPiece &str)
{
    ASSERT_TRUE(IsEqual(content,str));
    for (size_t pos1 = 0; pos1 <= content.size() + 3; ++pos1) {
        for (size_t pos2 = 0; pos2 <= content.size(); ++pos2) {
            for (size_t len = 0; len <= content.size() - pos2; ++len) {
                auto rc1 = content.find_first_of(str.const_raw_data() + pos2,pos1,len);
                int expected_rc = (rc1 == content.npos ? -1 : static_cast<int>(rc1));

                auto rc2 = str.find_first_of(str.const_raw_data() + pos2,pos1,len);
                int actual_rc = (rc2 == str.npos ? -1 : static_cast<int>(rc2));
                ASSERT_EQ(expected_rc,actual_rc);
            }
        }

        size_t kFindContentSizeMax = content.size() + content.size() / 2;
        for (size_t find_content_size = 0; find_content_size <= kFindContentSizeMax; ++find_content_size) {
            const std::string kFindContent(CreateRandomStdString(find_content_size));

            auto rc1 = content.find_first_of(kFindContent.data(),pos1,kFindContent.size());
            int expected_rc = (rc1 == content.npos ? -1 : static_cast<int>(rc1));

            auto rc2 = str.find_first_of(kFindContent.data(),pos1,kFindContent.size());
            int actual_rc = (rc2 == str.npos ? -1 : static_cast<int>(rc2));
            ASSERT_EQ(expected_rc,actual_rc);
        }
    }
}

void TestFindLastOf(const std::string &content,const StringPiece &str)
{
    ASSERT_TRUE(IsEqual(content,str));
    for (size_t pos1 = 0; pos1 <= content.size() + 3; ++pos1) {
        for (size_t pos2 = 0; pos2 <= content.size(); ++pos2) {
            for (size_t len = 0; len <= content.size() - pos2; ++len) {
                auto rc1 = content.find_last_of(str.const_raw_data() + pos2,pos1,len);
                int expected_rc = (rc1 == content.npos ? -1 : static_cast<int>(rc1));

                auto rc2 = str.find_last_of(str.const_raw_data() + pos2,pos1,len);
                int actual_rc = (rc2 == str.npos ? -1 : static_cast<int>(rc2));
                ASSERT_EQ(expected_rc,actual_rc);
            }
        }

        size_t kFindContentSizeMax = content.size() + content.size() / 2;
        for (size_t find_content_size = 0; find_content_size <= kFindContentSizeMax; ++find_content_size) {
            const std::string kFindContent(CreateRandomStdString(find_content_size));

            auto rc1 = content.find_last_of(kFindContent.data(),pos1,kFindContent.size());
            int expected_rc = (rc1 == content.npos ? -1 : static_cast<int>(rc1));

            auto rc2 = str.find_last_of(kFindContent.data(),pos1,kFindContent.size());
            int actual_rc = (rc2 == str.npos ? -1 : static_cast<int>(rc2));
            ASSERT_EQ(expected_rc,actual_rc);
        }
    }
}

void TestFindFirstNotOf(const std::string &content,const StringPiece &str)
{
    ASSERT_TRUE(IsEqual(content,str));
    for (size_t pos1 = 0; pos1 <= content.size() + 3; ++pos1) {
        for (size_t pos2 = 0; pos2 <= content.size(); ++pos2) {
            for (size_t len = 0; len <= content.size() - pos2; ++len) {
                auto rc1 = content.find_first_not_of(str.const_raw_data() + pos2,pos1,len);
                int expected_rc = (rc1 == content.npos ? -1 : static_cast<int>(rc1));

                auto rc2 = str.find_first_not_of(str.const_raw_data() + pos2,pos1,len);
                int actual_rc = (rc2 == str.npos ? -1 : static_cast<int>(rc2));
                ASSERT_EQ(expected_rc,actual_rc);
            }
        }

        size_t kFindContentSizeMax = content.size() + content.size() / 2;
        for (size_t find_content_size = 0; find_content_size <= kFindContentSizeMax; ++find_content_size) {
            const std::string kFindContent(CreateRandomStdString(find_content_size));

            auto rc1 = content.find_first_not_of(kFindContent.data(),pos1,kFindContent.size());
            int expected_rc = (rc1 == content.npos ? -1 : static_cast<int>(rc1));

            auto rc2 = str.find_first_not_of(kFindContent.data(),pos1,kFindContent.size());
            int actual_rc = (rc2 == str.npos ? -1 : static_cast<int>(rc2));
            ASSERT_EQ(expected_rc,actual_rc);
        }
    }
}

void TestFindLastNotOf(const std::string &content,const StringPiece &str)
{
    ASSERT_TRUE(IsEqual(content,str));
    for (size_t pos1 = 0; pos1 <= content.size() + 3; ++pos1) {
        for (size_t pos2 = 0; pos2 <= content.size(); ++pos2) {
            for (size_t len = 0; len <= content.size() - pos2; ++len) {
                auto rc1 = content.find_last_not_of(str.const_raw_data() + pos2,pos1,len);
                int expected_rc = (rc1 == content.npos ? -1 : static_cast<int>(rc1));

                auto rc2 = str.find_last_not_of(str.const_raw_data() + pos2,pos1,len);
                int actual_rc = (rc2 == str.npos ? -1 : static_cast<int>(rc2));
                ASSERT_EQ(expected_rc,actual_rc);
            }
        }

        size_t kFindContentSizeMax = content.size() + content.size() / 2;
        for (size_t find_content_size = 0; find_content_size <= kFindContentSizeMax; ++find_content_size) {
            const std::string kFindContent(CreateRandomStdString(find_content_size));

            auto rc1 = content.find_last_not_of(kFindContent.data(),pos1,kFindContent.size());
            int expected_rc = (rc1 == content.npos ? -1 : static_cast<int>(rc1));

            auto rc2 = str.find_last_not_of(kFindContent.data(),pos1,kFindContent.size());
            int actual_rc = (rc2 == str.npos ? -1 : static_cast<int>(rc2));
            ASSERT_EQ(expected_rc,actual_rc);
        }
    }
}

STRING_PIECE_TEST(Find)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            const StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            TestFind(content,str);
        }
    }
}

STRING_PIECE_TEST(RFind)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            const StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            TestRFind(content,str);
        }
    }
}

STRING_PIECE_TEST(FindFirstOf)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            const StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            TestFindFirstOf(content,str);
        }
    }
}


STRING_PIECE_TEST(FindLastOf)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            const StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            TestFindLastOf(content,str);
        }
    }
}


STRING_PIECE_TEST(FindFirstNotOf)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            const StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            TestFindFirstNotOf(content,str);
        }
    }
}


STRING_PIECE_TEST(FindLastNotOf)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            const StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            TestFindLastNotOf(content,str);
        }
    }
}

void TestCompare(const std::string &content,const StringPiece &str)
{
    ASSERT_TRUE(IsEqual(content,str));
    for (size_t pos1 = 0; pos1 <= content.size(); ++pos1) {
        for (size_t len1 = 0; len1 <= (content.size() - pos1) + 3; ++len1) {
            for (size_t len2 = 0; len2 <= content.size() + 3; ++len2) {
                std::string compare_content(CreateRandomStdString(len2));
                int expected_rc = content.compare(pos1,len1,compare_content.data(),compare_content.size());
                int actual_rc = str.compare(pos1,len1,compare_content.data(),compare_content.size());
                if (expected_rc < 0)
                    ASSERT_LT(actual_rc,0);
                else if (expected_rc == 0)
                    ASSERT_EQ(0,actual_rc);
                else
                    ASSERT_GT(actual_rc,0);
            }
        }
    }
}

STRING_PIECE_TEST(Compare)
{
    for (size_t buffer_size = 0; buffer_size <= 100; ++buffer_size) {
        ExtendedStdString buffer(buffer_size,'x');

        for (size_t content_size = 0; content_size <= buffer_size; ++content_size) {
            const std::string content(CreateRandomStdString(content_size));
            const StringPiece str(buffer.raw_data(),buffer_size,content.data(),content.size());

            TestCompare(content,str);
        }
    }
}
