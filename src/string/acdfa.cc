
#include <queue>

#include "string/ctype.h"
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
    size_t pattern_idx = 0;
    state_t state = 0;

    while (pattern_idx < size && goto_(state, data[pattern_idx]) != kNANState) {
        state = goto_(state, data[pattern_idx]);
        ++pattern_idx;
    }

    for (; pattern_idx < size; ++pattern_idx) {
        state_t newstate = goto_.rows.size();
        goto_.rows.emplace_back();

        const uint8_t ch = data[pattern_idx];
        const uint8_t upper_ch = ToUpper(ch);
        const uint8_t lower_ch = ToLower(ch);
        goto_(state, upper_ch) = newstate;
        if (lower_ch != upper_ch) {
            goto_(state, lower_ch) = newstate;
        }

        state = newstate;
    }

    size_t status_num = state + static_cast<size_t>(1);
    if (output_.size() < status_num) {
        output_.resize(status_num);
    }
    output_[state].push_back(size);

    return ;
}

void ACDFA::FillFailAndOutput(const uint8_t * const alphabet_ptr, const size_t alphabet_size) {
    using queue_t = std::queue<state_t>;

    queue_t state_queue;
    for (size_t idx = 0; idx < alphabet_size; ++idx) {
        state_t next_state = goto_(0, alphabet_ptr[idx]);
        if (next_state != kNANState) {
            state_queue.push(next_state);
        }
    }

    while (!state_queue.empty()) {
        const state_t r = state_queue.front();
        state_queue.pop();

        for (size_t idx = 0; idx < alphabet_size; ++idx) {
            const uint8_t ch = alphabet_ptr[idx];
            const state_t s = goto_(r, ch);
            if (s != kNANState) {
                state_queue.push(s);
                state_t fr = fail_[r];
                while (fr != 0 && goto_(fr, ch) == kNANState) {
                    fr = fail_[fr];
                }

                const state_t fr_next = goto_(fr, ch);
                if (fr_next != kNANState) {
                    fail_[s] = fr_next;

                    auto &s_output = output_[s];
                    const auto &fr_next_output = output_[fr_next];
                    s_output.insert(s_output.cend(), fr_next_output.cbegin(), fr_next_output.cend());
                }
            }
        }
    }

    return ;
}

namespace {

constexpr uint8_t kCSAlphabetTable[] {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
   32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
   48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
   64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
   80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
   96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
  112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
  128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
  144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
  160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
  192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
  208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
  224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
  240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};
constexpr size_t kCSAlphabetTableSize = sizeof(kCSAlphabetTable) / sizeof(kCSAlphabetTable[0]);


constexpr uint8_t kCISAlphabetTable[] {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
   32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
   48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
   64,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103, 104, 105,
  106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
  122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137,
  138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153,
  154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
  170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185,
  186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201,
  202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217,
  218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233,
  234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
  250, 251, 252, 253, 254, 255
};
constexpr size_t kCISAlphabetTableSize = sizeof(kCISAlphabetTable) / sizeof(kCISAlphabetTable[0]);

} // namespace

void ACDFA::FillFailAndOutputCS() {
    FillFailAndOutput(&kCSAlphabetTable[0], kCSAlphabetTableSize);
    return ;
}

void ACDFA::FillFailAndOutputCIS() {
    FillFailAndOutput(&kCISAlphabetTable[0], kCISAlphabetTableSize);
    return ;
}

