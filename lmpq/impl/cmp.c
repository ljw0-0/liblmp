#include "lmp.h"

typedef struct {
    const limb_t *a, *b;
    size_t an, bn, i;
    limb_t carry_lo, carry_hi;
} lmpq_product_t;

static limb_t lmpq_product_next(lmpq_product_t *p) {
    size_t n = p->an + p->bn;
    if (p->i >= n) return 0;

    size_t k = p->i++;
    limb_t lo = p->carry_lo, mid = p->carry_hi, hi = 0;
    size_t first = k >= p->bn ? k - (p->bn - 1) : 0;
    size_t last = k < p->an ? k : p->an - 1;

    if (first <= last) {
        for (size_t i = first; i <= last; ++i) {
            limb_t product_hi, product_lo;
            limb_mul(&product_hi, &product_lo, p->a[i], p->b[k-i]);

            limb_t old = lo;
            lo += product_lo;
            limb_t carry = lo < old;

            old = mid;
            mid += product_hi;
            limb_t carry_hi = mid < old;
            old = mid;
            mid += carry;
            carry_hi += mid < old;
            hi += carry_hi;
        }
    }

    p->carry_lo = mid;
    p->carry_hi = hi;
    return lo;
}

static int lmpq_cmp_abs_products(const lmpz_t a, const lmpz_t b, const lmpz_t c, const lmpz_t d) {
    lmpq_product_t x = {
        lmpz_get_mpn(a), lmpz_get_mpn(b), lmpz_size(a), lmpz_size(b), 0, 0, 0
    };
    lmpq_product_t y = {
        lmpz_get_mpn(c), lmpz_get_mpn(d), lmpz_size(c), lmpz_size(d), 0, 0, 0
    };
    size_t xn = x.an + x.bn, yn = y.an + y.bn;
    size_t n = xn > yn ? xn : yn;
    int cmp = 0;

    for (size_t i = 0; i < n; ++i) {
        limb_t xl = lmpq_product_next(&x), yl = lmpq_product_next(&y);
        if (xl != yl) cmp = xl < yl ? -1 : 1;
    }
    return cmp;
}

int _lmpq_cmp(const lmpz_t ap, const lmpz_t aq, const lmpz_t bp, const lmpz_t bq) {
    assert(!lmpz_is_zero(aq) && !lmpz_is_zero(bq));

    int sign_a = lmpz_is_zero(ap) ? 0 :
        ((lmpz_sign_bit(ap) ^ lmpz_sign_bit(aq)) ? -1 : 1);
    int sign_b = lmpz_is_zero(bp) ? 0 :
        ((lmpz_sign_bit(bp) ^ lmpz_sign_bit(bq)) ? -1 : 1);
    if (sign_a != sign_b) return sign_a < sign_b ? -1 : 1;
    if (!sign_a) return 0;

    lmpz_t x, y;
    lmpz_init(x);
    lmpz_init(y);
    int err = lmpz_mul(x, ap, bq);
    if (!err) err = lmpz_mul(y, aq, bp);

    int cmp;
    if (!err) {
        cmp = lmpz_cmp(x, y);
        if (lmpz_sign_bit(aq) ^ lmpz_sign_bit(bq)) cmp = -cmp;
    } else {
        lmpz_clear(x);
        lmpz_clear(y);
        cmp = lmpq_cmp_abs_products(ap, bq, bp, aq);
        if (sign_a < 0) cmp = -cmp;
        return cmp;
    }
    lmpz_clear(x);
    lmpz_clear(y);
    return cmp;
}

int _lmpq_cmp_z(const lmpz_t ap, const lmpz_t aq, const lmpz_t b) {
    assert(!lmpz_is_zero(aq));

    int sign_a = lmpz_is_zero(ap) ? 0 :
        ((lmpz_sign_bit(ap) ^ lmpz_sign_bit(aq)) ? -1 : 1);
    int sign_b = lmpz_is_zero(b) ? 0 : (lmpz_sign_bit(b) ? -1 : 1);
    if (sign_a != sign_b) return sign_a < sign_b ? -1 : 1;
    if (!sign_a) return 0;

    lmpz_t product;
    lmpz_init(product);
    if (!lmpz_mul(product, aq, b)) {
        int cmp = lmpz_cmp(ap, product);
        if (lmpz_sign_bit(aq)) cmp = -cmp;
        lmpz_clear(product);
        return cmp;
    }
    lmpz_clear(product);

    lmpz_t one;
    lmpz_init(one);
    lmpz_one(one);
    int cmp = lmpq_cmp_abs_products(ap, one, aq, b);
    if (sign_a < 0) cmp = -cmp;
    return cmp;
}

/*
*/