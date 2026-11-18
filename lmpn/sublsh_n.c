#include "lmp.h"

limb_t lmpn_sublsh_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n, int cnt) {
    limb_t lo, sum, d, borrow=0;

    do {
        lo = *b << cnt;

        sum = *a - lo;
        d = sum - borrow;

        borrow = (*b >> (LIMB_WIDTH - cnt)) + (sum > *a) + (sum < borrow);
        *c = d;

        ++b;
        ++a;
        ++c;
    } while (--n);
    
    return borrow;
}