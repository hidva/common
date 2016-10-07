#include "concurrent/mutex.h"

#include <gtest/gtest.h>



TEST(SharedMutexTest, test) {
    std::shared_mutex mux;
    mux.lock();
    EXPECT_FALSE(mux.try_lock());
    mux.unlock();

    EXPECT_TRUE(mux.try_lock());
    mux.unlock();

    mux.lock_shared();
    EXPECT_TRUE(mux.try_lock_shared());
    mux.unlock_shared();
    mux.unlock_shared();
}

