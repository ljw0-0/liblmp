#include "lmp.h"

void lmpn_divexact_by3(limb_t *q, const limb_t *a, size_t n) {
    limb_t borrow = 0;

    assert(n >= 1);

    do {
        limb_t s = *a, l, hi, lo;
        limb_t t = s - borrow;        /* 先做减法 */
        borrow = (s < borrow);        /* 减法产生的借位 */
        l = t * limb_inv3;            /* 乘逆元 */
        /* limb_mul(&hi, &lo, l, 3) 的化简：3*l = (l<<1) + l */
        lo = l + (l << 1);
        hi = (l >> (LIMB_WIDTH - 1)) + (lo < l);  /* 进位链 */
        *q = l;
        borrow += hi + (lo > t);      /* 回乘产生的借位 */
        ++a; ++q;
    } while (--n);
}
