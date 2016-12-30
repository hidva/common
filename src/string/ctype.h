#pragma once

#include <stdint.h>

/* 之所以没有使用标准库自带的 ctype.h, 是因为标准库 ctype 在实现时考虑了 Locale 因素, 势必会带来一些性能影响.
 */

/**
 * 若 ch 的值, 按照 ASCII 编码规则是小写字母的值, 则返回其对应大写字母的 ASCII 值.
 * 否则原样返回.
 */
uint8_t ToUpper(uint8_t ch) noexcept;

/**
 * 若 ch 的值, 按照 ASCII 编码规则是大写字母的值, 则返回其对应小写字母的 ASCII 值.
 * 否则原样返回.
 */
uint8_t ToLower(uint8_t ch) noexcept;

