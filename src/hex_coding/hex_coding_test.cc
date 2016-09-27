#include "hex_coding.h"
#include "gtest/gtest.h"
#include "exception/errno_exception.h"
#include "string/extended_std_string.h"

#include <string>
#include <string.h>

TEST(HexEncodeTest,test1)
{
    {
        unsigned char data[] {0x01};
        char buf[3];
        size_t size = HexEncode(buf,sizeof(buf),data,sizeof(data));
        ASSERT_EQ((size_t)2,size);
        buf[size] = '\0';
        EXPECT_STREQ("01",buf);

        ExtendedStdString str_buf;
        HexEncode(str_buf,data,sizeof(data));
        EXPECT_EQ(std::string{"01"},str_buf);

        str_buf = "Hello";
        HexEncode(str_buf,data,sizeof(data));
        EXPECT_EQ(std::string{"Hello01"},str_buf);

        EXPECT_EQ(std::string{"01"},HexEncode<ExtendedStdString>(data,sizeof(data)));
    }

    {
        unsigned char data[] {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
        char buf[2 * sizeof(data) + 1];
        size_t size = HexEncode(buf,sizeof(buf),data,sizeof(data));
        ASSERT_EQ(2 * sizeof(data),size);
        buf[size] = '\0';
        EXPECT_STREQ("0102030405060708090A0B0C0D0E0F",buf);

        ExtendedStdString str_buf;
        HexEncode(str_buf,data,sizeof(data));
        EXPECT_EQ(std::string{"0102030405060708090A0B0C0D0E0F"},str_buf);

        str_buf = "Hello";
        HexEncode(str_buf,data,sizeof(data));
        EXPECT_EQ(std::string{"Hello0102030405060708090A0B0C0D0E0F"},str_buf);

        EXPECT_EQ(std::string{"0102030405060708090A0B0C0D0E0F"},HexEncode<ExtendedStdString>(data,sizeof(data)));
    }
}

TEST(HexDecodeTest,test1)
{
    unsigned char data[128];
    EXPECT_THROW(HexDecode(data,sizeof(data),"01020X",3),pp_qq::ErrnoException);
    EXPECT_THROW(HexDecode(data,sizeof(data),"01020X",6),pp_qq::ErrnoException);

    constexpr unsigned char expected_val[] {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0};
    const char *hex_str = "0102030405060708090a0b0c0d0e0fA0B0C0D0E0F0";
    const size_t hex_str_size = strlen(hex_str);
    size_t data_size = HexDecode(data,sizeof(data),hex_str,hex_str_size);
    ASSERT_EQ(sizeof(expected_val),data_size);
    EXPECT_EQ(0,memcmp(expected_val,data,sizeof(expected_val)));

    ExtendedStdString str;
    HexDecode(str,hex_str,hex_str_size);
    EXPECT_EQ(std::string(reinterpret_cast<const char*>(expected_val),sizeof(expected_val)),str);

    str = "HelloWorld";
    HexDecode(str,hex_str,hex_str_size);
    ExtendedStdString expected_val_2 {"HelloWorld"};
    expected_val_2.append(reinterpret_cast<const char*>(expected_val),sizeof(expected_val));
    EXPECT_EQ(expected_val_2,str);

    EXPECT_EQ(std::string(reinterpret_cast<const char*>(expected_val),sizeof(expected_val)),HexDecode<ExtendedStdString>(hex_str,hex_str_size));
}





