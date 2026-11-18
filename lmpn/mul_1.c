#include "lmp.h"

limb_t lmpn_mul_1(limb_t *c, const limb_t *a, size_t n, limb_t b) {
    limb_t hi, lo, carry=0, sum;
    do {
        limb_mul(&hi, &lo, *a, b);
        ++a;
        sum = lo + carry;
        carry = hi + (sum < lo);
        *c = sum;
        ++c;
    } while (--n);
    return carry;
}