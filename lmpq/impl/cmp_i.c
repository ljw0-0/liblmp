#include "lmp.h"

static int lmpq_cmp_abs_mul_ui(const lmpz_t a, const lmpz_t b, limb_t c) {
    const limb_t *ap = lmpz_get_mpn(a), *bp = lmpz_get_mpn(b);
    size_t an = lmpz_size(a), bn = lmpz_size(b);
    size_t n = an > bn + 1 ? an : bn + 1;
    limb_t carry = 0;
    int cmp = 0;

    for (size_t i = 0; i < n; ++i) {
        limb_t product;
        if (i < bn) {
            limb_t hi, lo;
            limb_mul(&hi, &lo, bp[i], c);
            product = lo + carry;
            carry = hi + (product < lo);
        } else {
            product = i == bn ? carry : 0;
        }

        limb_t limb = i < an ? ap[i] : 0;
        if (limb != product) cmp = limb < product ? -1 : 1;
    }
    return cmp;
}

static int lmpq_cmp_si_magnitude(const lmpz_t ap, const lmpz_t aq, limb_t b, int sign_b) {
    assert(!lmpz_is_zero(aq));

    int sign_a = lmpz_is_zero(ap) ? 0 :
        ((lmpz_sgn_bit(ap) ^ lmpz_sgn_bit(aq)) ? -1 : 1);
    if (sign_a != sign_b) return sign_a < sign_b ? -1 : 1;
    if (!sign_a) return 0;

    int cmp = lmpq_cmp_abs_mul_ui(ap, aq, b);
    return sign_a < 0 ? -cmp : cmp;
}

int _lmpq_cmp_ui(const lmpz_t ap, const lmpz_t aq, limb_t b) {
    return lmpq_cmp_si_magnitude(ap, aq, b, b ? 1 : 0);
}

int _lmpq_cmp_si(const lmpz_t ap, const lmpz_t aq, slimb_t b) {
    limb_t magnitude = b < 0 ? (limb_t)0 - (limb_t)b : (limb_t)b;
    return lmpq_cmp_si_magnitude(ap, aq, magnitude, b < 0 ? -1 : b > 0 ? 1 : 0);
}
