#include "lmp.h"

int lmpn_subrsh2_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n) {
    int borrow = 0;
    limb_t d, sum;
    while (--n) {
        d = *a; sum = d - (*b >> 2 | (b[1] << (LIMB_WIDTH - 2)));
        *c = sum - borrow;
        borrow = (sum > d) | (*c > sum);
        ++a; ++b; ++c;
    }
    d = *a; sum = d - (*b >> 2);
    *c = sum - borrow;
    return (sum > d) | (*c > sum);
}