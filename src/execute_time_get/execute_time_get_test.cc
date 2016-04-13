
#include "execute_time_get/execute_time_get.h"
#include "gtest/gtest.h"

TEST(ExecuteTimeGetTest,test)
{
    ExecuteTimeGet(1,1,[] {});
    ExecuteTimeGet(1,1,[] {});
}


