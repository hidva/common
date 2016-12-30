#include "string/ctype.h"
#include "string/kmpdfa.h"

/* 使用标号 i 从 1 开始对 pattern 中每个字符进行标号. 这样 pattern 形成的 trie 树如下:
 *
 *  0 --a1--> 1 --a2--> 2 --...-- n-1 --an--> an
 *
 * 所以主要就是要注意这里对 pattern 的标号是从 1 开始的, 因为 0 被用作是初始状态.
 */

void KMPDFA::DoCompileCS() noexcept {
    state_t t = 0;
    for (state_t s = 1; s < (state_t)pattern_.size(); ++s) {
        while (t != 0 && pattern_[t] != pattern_[s]) {
            t = fail_[t];
        }

        if (pattern_[t] == pattern_[s]) {
            ++t;
            fail_[s + 1] = t;
        }
    }
    return ;
}

void KMPDFA::DoCompileCIS() noexcept {
    state_t t = 0;
    for (state_t s = 1; s < (state_t)pattern_.size(); ++s) {
        while (t != 0 && ToUpper(pattern_[t]) != ToUpper(pattern_[s])) {
            t = fail_[t];
        }

        if (ToUpper(pattern_[t]) == ToUpper(pattern_[s])) {
            ++t;
            fail_[s + 1] = t;
        }
    }
    return ;
}



size_t KMPDFA::RunCS(const char *data, size_t size) const noexcept {
    state_t s = 0;

    for (size_t i = 0; i < size; ++i) {
        while (s != 0 && pattern_[s] != data[i]) {
            s = fail_[s];
        }

        if (pattern_[s] == data[i]) {
            ++s;

            if (s == (state_t)pattern_.size()) {
                return (i + 1) - pattern_.size();
            }
        }
    }

    return size;
}

size_t KMPDFA::RunCIS(const char *data, size_t size) const noexcept {
    state_t s = 0;

    for (size_t i = 0; i < size; ++i) {
        while (s != 0 && ToUpper(pattern_[s]) != ToUpper(data[i])) {
            s = fail_[s];
        }

        if (ToUpper(pattern_[s]) == ToUpper(data[i])) {
            ++s;

            if (s == (state_t)pattern_.size()) {
                return (i + 1) - pattern_.size();
            }
        }
    }

    return size;
}


const KMPDFA::CompileFP KMPDFA::g_compile_func[] {&KMPDFA::DoCompileCS, &KMPDFA::DoCompileCIS};
const KMPDFA::RunFP KMPDFA::g_run_func[] {&KMPDFA::RunCS, &KMPDFA::RunCIS};
