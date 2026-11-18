#include "lmp.h"

limb_t lmpn_div_r_1(const limb_t *a, size_t n, limb_t b) {
    limb_t ans, base;

    assert(b);
    if (!(b & (b-1))) return a[0] % b;

    a += n-1;

    ans = *a % b;
    base = LIMB_MAX % b + 1;

    while (--n) {
        limb_t hi, lo;
        limb_mul(&hi, &lo, ans, base);
        lo += *(--a);
        hi += (lo < *a);
        ans = limb_divmod(&hi, hi, lo, b);
    }

    return ans;
}