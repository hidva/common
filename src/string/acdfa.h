
#pragma once

#include <string.h>
#include <stdint.h>

#include <initializer_list>
#include <string>
#include <vector>
#include <utility>

/**
 * ACDFA. Aho-Corasick 算法实现.
 *
 * ACDFA 的使用方式, 如下:
 *
 *      ACDFA acdfa;
 *      acdfa.Compile(...); // 编译指定的 pattern 集合.
 *      acdfa.Run(...); // 运行 DFA 查找输入串.
 *
 * 注意: 当 pattern 集合中所有 pattern 字节长度之和 > (2 的 (sizeof(state_t) * 8) - 2) 时, 将可能会触发
 * 未定义行为.
 *
 * 实现参考: http://note.youdao.com/noteshare?id=40bfbd9acf84d6de7ba5123fc66413e6.
 */
struct ACDFA {
private:
    struct StringRef {
        StringRef() = default;

        StringRef(const char *data, size_t size) noexcept:
            data_(data),
            size_(size) {
        }

        StringRef(const char *data) noexcept :
            StringRef(data, strlen(data)) {
        }

        StringRef(const std::string &str) noexcept :
            StringRef(str.data(), str.size()) {
        }

        const char* data() const noexcept {
            return data_;
        }

        size_t size() const noexcept {
            return size_;
        }


    private:
        const char *data_ = nullptr;
        size_t size_ = 0;
    };

public:
    ACDFA() = default;

    template <typename Iter>
    ACDFA(Iter first, Iter last, bool case_insensitive = false) {
        Compile(first, last, case_insensitive);
    }

    ACDFA(const std::initializer_list<StringRef> &patterns, bool case_insensitive = false) {
        Compile(patterns, case_insensitive);
    }

    /**
     * 编译给定的 pattern 集合得到 ACDFA.
     *
     * Compile() 不是线程安全的. 即不能在多个线程同时调用 Compile(); 或者同时调用 Compile(), Run().
     *
     * @tparam Iter 如下:
     *
     * 设 type 为 iterator_traits<Iter>::value_type, 那么 type 应满足如下 concept:
     *
     *      // 一切均字如其名.
     *      concept StringRef {
     *          const char* data() const noexcept;
     *          size_t size() const noexcept;
     *      };
     *
     * @param case_insensitive 若为 true, 则表明使用大小写无关的方式来编译模式串, 之后在 Run() 时将使用大小写无关
     *  匹配. 若为 false, 则相反.
     */
    template <typename Iter>
    void Compile(Iter first, Iter last, bool case_insensitive = false);

    void Compile(const std::initializer_list<StringRef> &patterns, bool case_insensitive = false) {
        Compile(patterns.begin(), patterns.end(), case_insensitive);
        return ;
    }

    /**
     * 运行 DFA 在 data, size 中查找 pattern.
     *
     * Run() 是线程安全的, 即可以在多个线程上同时调用同一个对象的 Run() 方法来进行匹配.
     *
     * @param on_accept 当查找到一个 pattern 之后的回调函数. 其调用形式如下:
     *
     *      // 其中 [data + start, data + start + size) 为查找到的 pattern.
     *      // @return 0, 表明继续查找; 1, 表明终止查找.
     *      int on_accept(size_t start, size_t size);
     */
    template <typename F>
    void Run(const char *data, size_t size, F &&on_accept) const;

    template <typename F>
    void Run(const char *data, F &&on_accept) const {
        Run(data, strlen(data), std::forward<F>(on_accept));
        return ;
    }

    template <typename F>
    void Run(const std::string &str, F &&on_accept) const {
        Run(str.data(), str.size(), std::forward<F>(on_accept));
        return ;
    }

    std::vector<std::pair<size_t, size_t>> Run(const char *data, size_t size) const {
        std::vector<std::pair<size_t, size_t>> pattern_idxes;

        Run(data, size, [&] (size_t start, size_t size) -> int {
            pattern_idxes.emplace_back(start, size);
            return 0;
        });

        return std::move(pattern_idxes);
    }

    std::vector<std::pair<size_t, size_t>> Run(const char *data) const {
        return Run(data, strlen(data));
    }

    std::vector<std::pair<size_t, size_t>> Run(const std::string &str) const {
        return Run(str.data(), str.size());
    }

private:
    using state_t = uint32_t;

    static constexpr state_t kNANState {0xFFFFFFFFU};

    // TrieTranRow 表示是 Trie 的状态转换表中的一行内容, 没错其中 TrieTran 表示 Trie 树的状态转换表.
    // 话说回来了, sizeof(TrieTranRow) 应该有 1k 了吧, 挺费内存的. 后续可以换成 std::unordered_map.
    struct TrieTranRow {
        state_t tran[256] {
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState,
            kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState, kNANState
        };
    };

    // TrieTran, Trie 树的状态转换表; rows[i] 为状态 i 上存在的转换. 对于状态 s, 符号 a, 其对应的下一个转化为 rows[s].tran[a].
    struct TrieTran {
        std::vector<TrieTranRow> rows;

    public:
        state_t operator () (state_t s, uint8_t symbol) const noexcept {
            return rows[s].tran[symbol];
        }
    };

private:
    TrieTran goto_;
    std::vector<std::vector<size_t>> output_;
    std::vector<state_t> fail_;


};
