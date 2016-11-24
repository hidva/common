
#pragma once

#include <unordered_map>
#include <list>

#include <mutex>
#include <vector>

#include "rrid/scope_exit.h"


/**
 * LRUKVCache.
 *
 * LRUKVCache 内部使用 unordered_map 来对 key 进行索引, Hash, KeyEqual 用来实例化 std::unordered_map, 按照
 * 语义上讲, Hash, KeyEqual 作为实现细节不应该暴露的, 但是木办法了.
 */
template <typename Key, typename T, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
struct LRUKVCache {
private:
    static constexpr size_t kDefaultBucketCount = 128;

public:
    using value_type = std::pair<const Key, T>;
    using list_t = std::list<value_type>;
    using iterator = typename list_t::iterator;

public:
    /**
     * 构造一个 LRUKVCache.
     *
     * @param max_size 指定了当前 Cache 中所允许的 key 的最大数目. 当超过此数目时, 将会移除一些老键.
     *  max_size 应该 > 0, 否则行为未定义.
     *
     * @param bucket_count, hash, equal 用来初始化 LRUKVCache 内部使用的 std::unordered_map.
     */
    LRUKVCache(size_t max_size, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual(), size_t bucket_count = kDefaultBucketCount):
        max_size_(max_size),
        items_index_(bucket_count, HashHelper(hash), KeyEqualHelper(equal)) {
    }

    LRUKVCache(const LRUKVCache &) = default;
    LRUKVCache& operator=(const LRUKVCache &) = default;

    LRUKVCache(LRUKVCache &&other):
        max_size_(std::move(other.max_size_)),
        items_(std::move(other.items_)),
        items_index_(std::move(other.items_index_)) {
    }

    LRUKVCache& operator= (LRUKVCache &&other) {
        max_size_ = std::move(other.max_size_);
        items_ = std::move(other.items_);
        items_index_ = std::move(other.items_index_);
        return *this;
    }

    /**
     * 若 key 在 LRUKVCache 中存在, 则返回其对应值的地址; 若不存在, 则返回 nullptr.
     */
    T* Get(const Key &key) {
        auto iter = items_index_.find(&key);
        if (iter == items_index_.end()) {
            return nullptr;
        }

        items_.splice(items_.begin(), items_, iter->second);
        return &iter->second->second;
    }

    /**
     * 将 key, value 添加到 LRUKVCache 中.
     */
    template <typename KeyType, typename ValueType>
    T& Put(KeyType &&key, ValueType &&value) {
        items_.emplace_front(std::forward<KeyType>(key), std::forward<ValueType>(value));
        auto item_iter = items_.begin();
        const Key *keyptr = &item_iter->first;

        auto item_index_iter = items_index_.find(keyptr);
        if (item_index_iter != items_index_.end()) {
            Erase(item_index_iter);
        }
        items_index_.emplace(keyptr, item_iter);

        if (items_index_.size() > max_size_) {
            Erase(items_index_.find(&items_.back().first));
        }

        return item_iter->second;
    }

private:
    /* index_iter 指向着 items_index_ 中某元素. 该函数从 items_index_, items_ 中移除该元素.
     *
     * 之所以使用模板, 只是不想写那一长串类型而已.
     */
    template <typename IterType>
    void Erase(IterType index_iter) {
        auto item_iter = index_iter->second;
        items_index_.erase(index_iter);
        items_.erase(item_iter);
        return ;
    }

private:
    struct HashHelper {

        HashHelper(const Hash &hash_obj):
            hash_obj_(hash_obj) {
        }

        size_t operator() (const Key *key) const noexcept {
            return hash_obj_(*key);
        }

    private:
        Hash hash_obj_;
    };

    struct KeyEqualHelper {
        KeyEqualHelper(const KeyEqual &key_equal):
            key_equal_(key_equal) {
        }

        bool operator() (const Key *left, const Key *right) const noexcept {
            return key_equal_(*left, *right);
        }

    private:
        KeyEqual key_equal_;
    };

private:
    size_t max_size_;
    list_t items_;
    std::unordered_map<const Key*, iterator, HashHelper, KeyEqualHelper> items_index_;
};



/**
 * ConcurrentLRUKVCache. 除了构造函数之外其余均线程安全.
 *
 */
template <typename Key, typename T, typename Hash1 = std::hash<Key>, typename Hash2 = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
struct ConcurrentLRUKVCache {

    /**
     * 构造函数.
     *
     * @param max_size MaxSize, 所能容纳键的最大值.
     * @param concurrent_level 并发级别, 意味着同时 concurrent_level 个线程读写 ConcurrentLRUKVCache 相互之
     *  间不会有影响.
     */
    ConcurrentLRUKVCache(size_t max_size, size_t concurrent_level = 16, const Hash1 &hash1 = Hash1(), const Hash2 &hash2 = Hash2(), const KeyEqual &ke = KeyEqual()):
        hash1_(hash1),
        segments_(concurrent_level, Segment(max_size / concurrent_level + 1, hash2, ke)) {
    }

    T* Get(const Key &key) {
        return SegmentFor(key).Get(key);
    }

    template <typename KeyType, typename ValueType>
    T& Put(KeyType &&key, ValueType &&value) {
        return SegmentFor(key).Put(std::forward<KeyType>(key), std::forward<ValueType>(value));
    }

    /**
     * Get() 的批量版本.
     *
     * @param get_key 其调用原型如下:
     *
     *      从 [first, last) 中迭代器指向的元素中提取出键的地址.
     *      KeyPtr get_key(const Iter::value_type &value);
     *
     * @param on_value 其调用原型如下:
     *
     *      // 若 value 为 nullptr, 则表明 key 不存在.
     *      void on_value(const Key &key, T *value);
     *
     */
    template <typename Iter, typename F1, typename F2>
    void GetAll(Iter first, Iter last, F1 &&get_key, F2 &&on_value) {
        std::vector<std::vector<const Key*>> keys_indexs(segments_.size());

        for (auto iter = first; iter != last; ++iter) {
            auto keyptr = get_key(*iter);
            keys_indexs[Index(*keyptr)].push_back(std::move(keyptr));
        }

        for (size_t keys_idx = 0; keys_idx < keys_indexs.size(); ++keys_idx) {
            auto &keys = keys_indexs[keys_idx];

            if (!keys.empty()) {
                Segment &segment = segments_[keys_idx];

                segment.mux.lock();
                ON_SCOPE_EXIT (unlock) {
                    segment.mux.unlock();
                };

                for (const Key *keyptr : keys) {
                    on_value(*keyptr, segment.kv_cache.Get(*keyptr));
                }
            }
        }

        return ;
    }

    /**
     * Put() 的批量版本.
     *
     * @param get_key_value 其调用原型如下:
     *
     *      void get_key_value(Iter::value_type &item, Key *&key, T *&value);
     *      这里在将 *key, *value 放到 LRUKVCache 中时, 使用的是拷贝构造函数, 不过倒是可以给 PutAll 再加个模板参
     *      数, 使得可以使用 std::move(*key) 之类的.
     *
     */
    template <typename Iter, typename F>
    void PutAll(Iter first, Iter last, F &&get_key_value) {
        std::vector<std::vector<std::pair<Key*, T*>>> indexs_kvs(segments_.size());

        for (auto iter = first; iter != last; ++iter) {
            Key *keyptr;
            T *valueptr;
            get_key_value(*iter, keyptr, valueptr);
            indexs_kvs[Index(*keyptr)].emplace_back(keyptr, valueptr);
        }

        for (size_t indexs_idx = 0; indexs_idx < indexs_kvs.size(); ++indexs_idx) {
            auto &kvs = indexs_kvs[indexs_idx];

            if (!kvs.empty()) {
                Segment &segment = segments_[indexs_idx];

                segment.mux.lock();
                ON_SCOPE_EXIT (unlock) {
                    segment.mux.unlock();
                };

                for (const auto &kv : kvs) {
                    segment.kv_cache.Put(*kv.first, *kv.second);
                }
            }
        }

        return ;
    }

private:
    struct Segment {
        LRUKVCache<Key, T, Hash2, KeyEqual> kv_cache;
        std::mutex mux;

    public:
        Segment(size_t max_size, const Hash2& hash, const KeyEqual& equal):
            kv_cache(max_size, hash, equal) {
        }

        Segment(const Segment &other):
            kv_cache(other.kv_cache) {
        }

        T* Get(const Key &key) {
            mux.lock();
            ON_SCOPE_EXIT(unlock_1) {
                mux.unlock();
            };

            return kv_cache.Get(key);
        }

        template <typename KeyType, typename ValueType>
        T& Put(KeyType &&key, ValueType &&value) {
            mux.lock();
            ON_SCOPE_EXIT(unlock_1) {
                mux.unlock();
            };

            return kv_cache.Put(std::forward<KeyType>(key), std::forward<ValueType>(value));
        }
    private:
        Segment(Segment &&) = delete;
        Segment& operator= (const Segment &) = delete;
        Segment& operator= (Segment &&) = delete;
    };

private:
    size_t Index(const Key &key) const noexcept {
        /* 在 jdk1.8 的 ConcurrentHashMap 的实现中, 其使用取与运算代替取余运算.
         * 不清楚这能优化多少, 所以暂且不进行优化.
         */
        return hash1_(key) % segments_.size();
    }

    Segment& SegmentFor(const Key &key) {
        return segments_[Index(key)];
    }

private:
    Hash1 hash1_;
    std::vector<Segment> segments_;
};





