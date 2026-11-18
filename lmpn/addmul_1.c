#include "lmp.h"

limb_t lmpn_addmul_1(limb_t *c, const limb_t *a, size_t n, limb_t b) {
    limb_t hi, lo, sum, d, carry=0;

    do {
        limb_mul(&hi, &lo, *a, b);

        sum = *c + lo;
        d = sum + carry;

        carry = hi + (sum < lo) + (d < sum);
        *c = d;

        ++a;
        ++c;
    } while (--n);

    return carry;
}