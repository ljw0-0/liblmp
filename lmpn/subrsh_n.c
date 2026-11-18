#include "lmp.h"

int lmpn_subrsh_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n, int cnt) {
    int borrow = 0;
    limb_t d, sum;
    while (--n) {
        d = *a; sum = d - (*b >> cnt | (b[1] << (LIMB_WIDTH - cnt)));
        *c = sum - borrow;
        borrow = (sum > d) | (*c > sum);
        ++a; ++b; ++c;
    }
    d = *a; sum = d - (*b >> cnt);
    *c = sum - borrow;
    return (sum > d) | (*c > sum);
}