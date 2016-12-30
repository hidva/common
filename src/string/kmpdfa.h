#pragma once

#include <stdexcept>
#include <string>
#include <vector>

/**
 * DFA, 用来实现 KMP 算法.
 *
 * 该类的一般使用姿势; 如下:
 *
 *      KMPDFA kmpdfa;
 *      kmpdfa.Compile(); // 完成对模式串的编译.
 *      kmpdfa.Run(); // 对输入串进行匹配. 根据 Compile(), Run() 的文档可知在调用 Compile 之后
 *                    // 可以在任意线程同时调用 kmpdfa.Run() 进行匹配.
 */
struct KMPDFA {
    KMPDFA(const char *data, size_t size, bool case_insensitive = false) {
        Compile(data, size, case_insensitive);
    }

    KMPDFA(const std::string &pattern, bool case_insensitive = false) {
        Compile(pattern, case_insensitive);
    }

    KMPDFA(std::string &&pattern, bool case_insensitive = false) {
        Compile(std::move(pattern), case_insensitive);
    }

    /**
     * 编译一个模式串到 DFA.
     *
     * Compile() 不是线程安全的. 即不能在多个线程同时调用 Compile(); 或者同时调用 Compile(), Run().
     *
     * @param data, size 用于确定模式串的内容.
     * @param case_insensitive 若为 true, 则表明使用大小写无关的方式来编译模式串, 之后在 Run() 时将使用大小写无关
     *  匹配. 若为 false, 则相反.
     */
    void Compile(const char *data, size_t size, bool case_insensitive = false) {
        Compile(std::string{data, size}, case_insensitive);
        return ;
    }

    void Compile(const std::string &pattern, bool case_insensitive = false) {
        pattern_ = pattern;
        case_insensitive_ = case_insensitive;
        DoCompile();
        return ;
    }

    void Compile(std::string &&pattern, bool case_insensitive = false) {
        pattern_ = std::move(pattern);
        case_insensitive_ = case_insensitive;
        DoCompile();
        return ;
    }

    /**
     * 运行 Compile() 编译之后的 DFA 对输入串 data,size 进行匹配; 找出 pattern 第一次出现的位置.
     *
     * Run() 是线程安全的, 即可以在多个线程上同时调用同一个对象的 Run() 方法来进行匹配.
     *
     * @return 若返回 size, 则表明 pattern 在输入串中不存在. 否则返回 pattern 第一次出现的下标.
     */
    size_t Run(const char *data, size_t size) const noexcept {
        return (this->*run_func_)(data, size);
    }

    size_t Run(const char *data) const noexcept {
        return Run(data, strlen(data));
    }

    size_t Run(const std::string &data) const noexcept {
        return Run(data.data(), data.size());
    }

    /**
     * 返回当前 KMPDFA 所使用 pattern 的地址以及大小.
     *
     * 返回的 pattern 的生命周期依附于其所属 KMPDFA 对象的生命周期. 使用者只能以只读的方式来操作返回的地址区间.
     */
    const char* PatternData() const noexcept {
        return pattern_.data();
    }

    size_t PatternSize() const noexcept {
        return pattern_.size();
    }

private:
    // 大小写敏感, 或者不敏感的 Run(), 参见 Run() 实现.
    size_t RunCS(const char *data, size_t size) const noexcept;
    size_t RunCIS(const char *data, size_t size) const noexcept;

    using RunFP = decltype(&KMPDFA::Run);

    // 此时 pattern_, case_insensitive_ 已经初始化完毕. 此函数开始编译操作.
    void DoCompile() {
        if (pattern_.empty()) {
            throw std::runtime_error("KMPDFA; DoCompile; ERROR: Empty Pattern");
        }

        run_func_ = g_run_func[case_insensitive_];

        fail_.resize(0);
        fail_.resize(pattern_.size() + 1, 0);
        (this->*g_compile_func[case_insensitive_])();
        return ;
    }

    // 大小写敏感的 compile. 参见 DoCompile() 的实现.
    void DoCompileCS() noexcept;

    // 大小写不敏感的 compile. 参见 DoCompile() 的实现.
    void DoCompileCIS() noexcept;

private:
    using state_t = int;
    using CompileFP = decltype(&KMPDFA::DoCompile);

private:
    static constexpr CompileFP g_compile_func[] {&KMPDFA::DoCompileCS, &KMPDFA::DoCompileCIS};
    static constexpr RunFP g_run_func[] {&KMPDFA::RunCS, &KMPDFA::RunCIS};

private:
    std::string pattern_;
    std::vector<state_t> fail_;
    RunFP run_func_ = nullptr;
    bool case_insensitive_ = false;
};


