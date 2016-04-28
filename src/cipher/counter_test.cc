
#include "cipher/counter.h"
#include "gtest/gtest.h"
#include <arpa/inet.h>

TEST(CounterTest,Init)
{
    pp_qq::Counter counter;

    unsigned int number;

    counter.Init(&number,sizeof(number));
    EXPECT_EQ(0,ntohl(number));
}

TEST(CounterTest,Increase)
{
    pp_qq::Counter counter;

    for (unsigned int i = 0; i < 333; ++i) {
        unsigned int number;

        counter.Init(&number,sizeof(number));
        ASSERT_EQ(0,ntohl(number));

        counter.Increase(&number,sizeof(number),i);
        ASSERT_EQ(i,ntohl(number));
    }
}

TEST(CounterTest,Increase2)
{
    pp_qq::Counter counter;

    unsigned int expected_value = 0;
    unsigned int actual_val;
    counter.Init(&actual_val,sizeof(actual_val));

    ASSERT_EQ(expected_value,ntohl(actual_val));
    for (unsigned int i = 1; i < 333; ++i) {
        counter.Increase(&actual_val,sizeof(actual_val),i);
        ASSERT_EQ(expected_value += i,ntohl(actual_val));
    }
}


