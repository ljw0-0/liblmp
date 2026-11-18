#include "lmp.h"
#include <assert.h>

/* ------ log2(base) 的 20 位定点表 ------ */

#if LIMB_WIDTH == 16 || LIMB_WIDTH == 32 || LIMB_WIDTH == 64

static const uint32_t lmpn_LOG2_FIXED[63] = {
    [0] = 0,
    [1] = 0,
    [2] = 1048576,
    [3] = 1661954,
    [4] = 2097152,
    [5] = 2434719,
    [6] = 2710530,
    [7] = 2943725,
    [8] = 3145728,
    [9] = 3323908,
    [10] = 3483295,
    [11] = 3627477,
    [12] = 3759106,
    [13] = 3880193,
    [14] = 3992301,
    [15] = 4096672,
    [16] = 4194304,
    [17] = 4286016,
    [18] = 4372484,
    [19] = 4454275,
    [20] = 4531871,
    [21] = 4605679,
    [22] = 4676053,
    [23] = 4743299,
    [24] = 4807682,
    [25] = 4869437,
    [26] = 4928769,
    [27] = 4985861,
    [28] = 5040877,
    [29] = 5093963,
    [30] = 5145248,
    [31] = 5194852,
    [32] = 5242880,
    [33] = 5289431,
    [34] = 5334592,
    [35] = 5378444,
    [36] = 5421060,
    [37] = 5462508,
    [38] = 5502851,
    [39] = 5542146,
    [40] = 5580447,
    [41] = 5617801,
    [42] = 5654255,
    [43] = 5689851,
    [44] = 5724629,
    [45] = 5758626,
    [46] = 5791875,
    [47] = 5824409,
    [48] = 5856258,
    [49] = 5887450,
    [50] = 5918013,
    [51] = 5947970,
    [52] = 5977345,
    [53] = 6006160,
    [54] = 6034437,
    [55] = 6062196,
    [56] = 6089453,
    [57] = 6116229,
    [58] = 6142539,
    [59] = 6168399,
    [60] = 6193824,
    [61] = 6218829,
    [62] = 6243428,
};


#else  /* 非常见 LIMB_WIDTH：运行时整数 atanh 级数 */

/* 保守实现：每一步都向上取整，保证结果 >= ceil(2^20 * log2(base))。
 * 仅用于 LIMB_WIDTH 不是 16/32/64 的情形（例如 128 位 limb）。 */
static uint32_t lmpn_log2_fixed_slow(unsigned base) {
    unsigned k = 0, x = base;
    while (x > 1) { x >>= 1; k++; }

    /* base = 2^k * m, m ∈ [1,2)，M = m * 2^20 = base * 2^(20-k) */
    uint64_t M = (uint64_t)base << (20 - k);
    uint64_t D = (uint64_t)1 << 20;

    /* y = (M - D) / (M + D)，Y = ceil(y * 2^20) */
    uint64_t num = ((uint64_t)(M - D)) << 20;
    uint64_t den = M + D;
    uint64_t Y = (num + den - 1) / den;          /* 向上取整 */

    /* y2 = ceil(Y^2 / 2^20) */
    uint64_t y2 = ((Y * Y) + D - 1) >> 20;

    uint64_t term = Y, sum = Y;
    for (unsigned j = 1; j < 20; j++) {
        /* term = ceil(term * y2 / 2^20) */
        term = ((term * y2) + D - 1) >> 20;
        if (term == 0) break;
        /* sum += ceil(term / (2j+1)) */
        sum += (term + 2 * j) / (2 * j + 1);
    }

    /* log2(m) = 2/ln2 * atanh(y)
     * C = ceil(2/ln2 * 2^20) = 3025703 */
    const uint64_t C = 3025703ULL;
    uint64_t log2_m = ((sum * C) + D - 1) >> 20;

    return (uint32_t)((k << 20) + log2_m);
}

#endif

/* ------ 主函数 ------ */

/* 返回：len 个 base 进制数字（最大值为 base^len - 1）转为大整数
 * 所需的最小 limb 数的保守上界；若溢出则返回 SIZE_MAX。
 *
 * 精确公式：
 *   n = ceil(len * log2(base) / LIMB_WIDTH)
 *
 * 对 2 的幂分支，结果是精确值。
 * 对其它 base，由于使用向上取整的 2^20 定点 log2，结果是上界（可能多 1）。
 */
size_t lmpn_limbs_for_digits(size_t len, int base) {
    assert(len < LMPN_MAX_LEN);
    assert(base >= 2 && base <= 62);
    if (len == 0) return 0;

    /* base 是 2 的幂：精确整数计算 */
    if ((base & (base - 1)) == 0) {
        unsigned k = 0, b = (unsigned)base;
        while (b > 1) { b >>= 1; k++; }
        if (len > (SIZE_MAX - (LIMB_WIDTH - 1)) / k) return SIZE_MAX;
        return (len * k + LIMB_WIDTH - 1) / LIMB_WIDTH;
    }

#if LIMB_WIDTH >= 32

    /* n = ceil(len * log2(base) / LIMB_WIDTH)
     *   = ceil(len * Lf / (2^20 * LIMB_WIDTH))        */
    uint32_t Lf;
#  if LIMB_WIDTH == 32 || LIMB_WIDTH == 64
    Lf = lmpn_LOG2_FIXED[base];
#  else
    Lf = lmpn_log2_fixed_slow((unsigned)base);
#  endif

    /* den = 2^20 * LIMB_WIDTH */
    size_t den = ((size_t)1 << 20) * (size_t)LIMB_WIDTH;

    /* len 必须能放进 limb_t，否则 limb_mul 会截断 */
    if (len > (size_t)LIMB_MAX) return SIZE_MAX;

    /* 双字积 len * Lf，用 limb_mul 而非 int128 */
    limb_t hi, lo, q;
    limb_mul(&hi, &lo, (limb_t)len, (limb_t)Lf);

    /* hi >= den 说明商 >= 2^LIMB_WIDTH，溢出 */
    if (hi >= (limb_t)den) return SIZE_MAX;

    /* 双字除单字，余数用于上取整 */
    limb_t r = limb_divmod(&q, hi, lo, (limb_t)den);
    if (r != 0) q++;

    return (size_t)q;

#else  /* LIMB_WIDTH == 16：保守 ceil_log2 上界（表值超出 16 位 limb） */

    unsigned k = 0, b = (unsigned)base;
    while (b > 1) { b >>= 1; k++; }
    if (base & (base - 1)) k++;   /* ceil(log2(base)) */
    if (len > (SIZE_MAX - (LIMB_WIDTH - 1)) / k) return SIZE_MAX;
    return (len * k + LIMB_WIDTH - 1) / LIMB_WIDTH;

#endif
}