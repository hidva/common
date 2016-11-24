#include <string>
#include <gtest/gtest.h>
#include <iostream>

#include "common/lru.h"



TEST(LRUKVCache, test1) {
    LRUKVCache<std::string, std::string> lru_kv_cache(3);
    EXPECT_EQ(nullptr, lru_kv_cache.Get("hello"));
    EXPECT_EQ(nullptr, lru_kv_cache.Get("hello"));
    EXPECT_EQ(nullptr, lru_kv_cache.Get("hello"));
    lru_kv_cache.Put("hello", "a");
    EXPECT_EQ("a", *lru_kv_cache.Get("hello"));
    EXPECT_EQ("a", *lru_kv_cache.Get("hello"));
    EXPECT_EQ("a", *lru_kv_cache.Get("hello"));

    lru_kv_cache.Put("world", "b");
    EXPECT_EQ("b", *lru_kv_cache.Get("world"));
    EXPECT_EQ("a", *lru_kv_cache.Get("hello"));
    EXPECT_EQ("b", *lru_kv_cache.Get("world"));
    EXPECT_EQ("a", *lru_kv_cache.Get("hello"));
    EXPECT_EQ("a", *lru_kv_cache.Get("hello"));
    EXPECT_EQ("b", *lru_kv_cache.Get("world"));

    lru_kv_cache.Put("k1", "v1");
    lru_kv_cache.Put("k1", "v2");
    lru_kv_cache.Put("k1", "v3");
    lru_kv_cache.Put("k2", "v");
    EXPECT_EQ(nullptr, lru_kv_cache.Get("hello"));
    EXPECT_EQ("b", *lru_kv_cache.Get("world"));
    EXPECT_EQ("v3", *lru_kv_cache.Get("k1"));
    EXPECT_EQ("v", *lru_kv_cache.Get("k2"));
}

TEST(LRUKVCache, test2) {
    LRUKVCache<int, int> lru_kv_cache(3);
    EXPECT_EQ(nullptr, lru_kv_cache.Get(1));
    EXPECT_EQ(nullptr, lru_kv_cache.Get(1));
    EXPECT_EQ(nullptr, lru_kv_cache.Get(1));
    lru_kv_cache.Put(1, 2);
    EXPECT_EQ(2, *lru_kv_cache.Get(1));
    EXPECT_EQ(2, *lru_kv_cache.Get(1));
    EXPECT_EQ(2, *lru_kv_cache.Get(1));

    lru_kv_cache.Put(2, 3);
    EXPECT_EQ(3, *lru_kv_cache.Get(2));
    EXPECT_EQ(2, *lru_kv_cache.Get(1));
    EXPECT_EQ(3, *lru_kv_cache.Get(2));
    EXPECT_EQ(2, *lru_kv_cache.Get(1));
    EXPECT_EQ(2, *lru_kv_cache.Get(1));
    EXPECT_EQ(3, *lru_kv_cache.Get(2));

    lru_kv_cache.Put(3, 4);
    lru_kv_cache.Put(3, 5);
    lru_kv_cache.Put(3, 6);
    lru_kv_cache.Put(4, 13);
    EXPECT_EQ(nullptr, lru_kv_cache.Get(1));
    EXPECT_EQ(3, *lru_kv_cache.Get(2));
    EXPECT_EQ(6, *lru_kv_cache.Get(3));
    EXPECT_EQ(13, *lru_kv_cache.Get(4));
}

TEST(ConcurrentLRUKVCache, test1) {
    ConcurrentLRUKVCache<int, int> con_lrukv(4, 2);
    EXPECT_EQ(nullptr, con_lrukv.Get(1));
    EXPECT_EQ(nullptr, con_lrukv.Get(1));
    EXPECT_EQ(nullptr, con_lrukv.Get(1));
    con_lrukv.Put(1, 2);
    EXPECT_EQ(2, *con_lrukv.Get(1));
    EXPECT_EQ(2, *con_lrukv.Get(1));
    EXPECT_EQ(2, *con_lrukv.Get(1));

    con_lrukv.Put(2, 3);
    EXPECT_EQ(3, *con_lrukv.Get(2));
    EXPECT_EQ(2, *con_lrukv.Get(1));
    EXPECT_EQ(3, *con_lrukv.Get(2));
    EXPECT_EQ(2, *con_lrukv.Get(1));
    EXPECT_EQ(2, *con_lrukv.Get(1));
    EXPECT_EQ(3, *con_lrukv.Get(2));

    con_lrukv.Put(3, 4);
    con_lrukv.Put(3, 5);
    con_lrukv.Put(3, 6);
    EXPECT_EQ(6, *con_lrukv.Get(3));
    con_lrukv.Put(4, 41);
    con_lrukv.Put(4, 42);
    EXPECT_EQ(42, *con_lrukv.Get(4));
    con_lrukv.Put(5, 51);
    EXPECT_EQ(51, *con_lrukv.Get(5));
}

TEST(ConcurrentLRUKVCache, test2) {
    ConcurrentLRUKVCache<int, int> con_lrukv(4, 2);

    std::unordered_map<int, int> test_material {
        {1, 2}, {2, 3}, {3, 4}, {23, 43}, {324, 23},
        {4, 8}, {32, 43}, {33, 13}, {43, 32}, {123, 324}
    };

    con_lrukv.PutAll(test_material.cbegin(), test_material.cend(),
        [] (const std::pair<const int, int> &kv, int *&kptr, int *&vptr) noexcept {
            kptr = (int*)&kv.first;
            vptr = (int*)&kv.second;
            return ;
        });

    size_t null_key_num = 0;
    con_lrukv.GetAll(
        test_material.cbegin(), test_material.cend(),
        [] (const std::pair<const int, int> &kv) noexcept -> const int * {
            return &kv.first;
        },
        [&] (const int &key, int *value) {
            auto iter = test_material.find(key);
            ASSERT_NE(test_material.end(), iter);
            ASSERT_TRUE(value == nullptr || *value == iter->second);

            if (value == nullptr) {
                ++null_key_num;
            }

            // std::cout << "key: " << key << ", value: " << *value << std::endl;
        }
    );
    EXPECT_GE(null_key_num, 4U);
}

