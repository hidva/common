
#include "string/acdfa.h"

const ACDFA::CompilePatternFP ACDFA::compile_funcs[] {&ACDFA::CompilePatternCS, &ACDFA::CompilePatternCIS};
const ACDFA::FillFailAndOutputFP ACDFA::ffo_funcs[] {&ACDFA::FillFailAndOutputCS, &ACDFA::FillFailAndOutputCIS};

void ACDFA::CompilePatternCS(const char * const data, const size_t size) {
    size_t pattern_idx = 0;
    state_t state = 0;

    while (pattern_idx < size && goto_(state, data[pattern_idx]) != kNANState) {
        state = goto_(state, data[pattern_idx]);
        ++pattern_idx;
    }

    for (; pattern_idx < size; ++pattern_idx) {
        state_t newstate = goto_.rows.size();
        goto_.rows.emplace_back();
        goto_(state, data[pattern_idx]) = newstate;
        state = newstate;
    }

    size_t status_num = state + static_cast<size_t>(1);
    if (output_.size() < status_num) {
        output_.resize(status_num);
    }
    output_[state].push_back(size);

    return ;
}

void ACDFA::CompilePatternCIS(const char * const data, const size_t size) {

}

void ACDFA::FillFailAndOutput(const uint8_t * const alphabet_ptr, const size_t alphabet_size) {

}


void ACDFA::FillFailAndOutputCS() {

}

void ACDFA::FillFailAndOutputCIS() {

}

