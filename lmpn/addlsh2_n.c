#include "lmp.h"

limb_t lmpn_addlsh2_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n) {
    limb_t lo, sum, d, carry=0;
    do {
        lo = *b << 2;

        sum = *a + lo;
        d = sum + carry;

        carry = (*b >> (LIMB_WIDTH - 2)) + (sum < lo) + (d < sum);
        *c = d;

        ++a;
        ++b;
        ++c;
    } while (--n);
    return carry;
}