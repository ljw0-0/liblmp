#include "lmp.h"

limb_t lmpn_sublsh2_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n) {
    limb_t lo, sum, d, borrow=0;

    do {
        lo = *b << 2;

        sum = *a - lo;
        d = sum - borrow;

        borrow = (*b >> (LIMB_WIDTH - 2)) + (sum > *a) + (sum < borrow);
        *c = d;

        ++b;
        ++a;
        ++c;
    } while (--n);
    
    return borrow;
}