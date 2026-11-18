#include "lmp.h"

int lmpn_sub_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n) {
    int borrow = 0;
    do {
        limb_t sum = *a - *b, d = *a;
        *c = sum - borrow;
        borrow = (sum > d) | (sum < borrow);
        ++a; ++b; ++c;
    } while (--n);
    return borrow;
}
