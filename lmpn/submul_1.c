#include "lmp.h"

limb_t lmpn_submul_1(limb_t *c, const limb_t *a, size_t n, limb_t b) {
    limb_t hi, lo, sum, d, borrow=0;
    
    do {
        limb_mul(&hi, &lo, *a, b);

        sum = *c - lo;
        d = sum - borrow;

        borrow = hi + (sum > *c) + (sum < borrow);
        *c = d;

        ++a;
        ++c;
    } while (--n);
    
    return borrow;
}