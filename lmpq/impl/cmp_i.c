#include "lmp.h"

static int cmp(const limb_t *pp, size_t pn, const limb_t *qp, size_t qn, limb_t b) {
    // p/q cmp b <=> p cmp q*b
    if (qn > pn) return -1;
    if (qn+1 < pn) return 1;
    limb_t hi, lo, sum, borrow=0;
    int d = pn-qn;
    
    do {
        limb_mul(&hi, &lo, *qp, b);

        sum = *pp - lo;

        borrow = hi + (sum > *pp) + (sum < borrow);

        ++pp;
        ++qp;
    } while (--qn);

    return d ? limb_cmp(*pp, borrow) : (borrow ? -1 : !!sum);
}


int _lmpq_cmp_ui(const lmpz_t ap, const lmpz_t aq, limb_t b) {
    assert(!lmpz_is_zero(aq));
    if (lmpz_is_zero(ap)) return -!!b;
    return ((ap->size ^ aq->size) & LMPZ_SIGN_BIT) ? -1 : cmp(lmpz_get_mpn(ap), lmpz_size(ap), lmpz_get_mpn(aq), lmpz_size(aq), b);
}

int _lmpq_cmp_si(const lmpz_t ap, const lmpz_t aq, slimb_t b) {
    assert(!lmpz_is_zero(aq));
    if (lmpz_is_zero(ap)) return limb_sgn(b);
    return (((ap->size ^ aq->size) >= LMPZ_SIGN_BIT) ^ (b < 0)) ? cmp(lmpz_get_mpn(ap), lmpz_size(ap), lmpz_get_mpn(aq), lmpz_size(aq), b) : -cmp(lmpz_get_mpn(ap), lmpz_size(ap), lmpz_get_mpn(aq), lmpz_size(aq), -(limb_t)b);
}