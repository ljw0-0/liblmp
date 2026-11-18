#include "lmp.h"

void lmpn_divexact_1(limb_t *q, const limb_t *a, size_t n, limb_t b) {
    limb_t inv, borrow = 0;

    assert(b);

    if (!(b & 1)) {
        int ctz = limb_ctz(b);
        b >>= ctz;
        lmpn_rshift(q, a, n, ctz);
        a = q;
    }

    inv = limb_inv(b);

    do {
        limb_t s = *a, l, hi, lo;
        limb_t t = s - borrow;        // 先做减法
        borrow = (s < borrow);        // 减法产生的借位
        l = t * inv;                  // 乘逆元
        limb_mul(&hi, &lo, l, b);
        *q = l;
        borrow += hi + (lo > t);      // 回乘产生的借位
        ++a; ++q;
    } while (--n);
}