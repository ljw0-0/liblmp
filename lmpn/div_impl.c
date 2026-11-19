#include "lmp.h"
#include <string.h>

#define DIV_DC_THRESHOLD 200

static int lmpn_divmod_3n_2n(limb_t *q, limb_t *a, size_t n, const limb_t *b, size_t m);

static void lmpn_divmod_knuth(limb_t *q, limb_t *a, size_t n, const limb_t *b, size_t m) {
    for (size_t i=n-1;i>=m;--i) {
        q[i-m] = limb_div(a[i], a[i-1], b[m-1]);
        limb_t borrow = lmpn_submul_1(a+i-m, b, m, q[i-m]);
        while (borrow > a[i]) {
            --q[i-m];
            borrow -= lmpn_add_n(a+i-m, a+i-m, b, m);
        }
        a[i] -= borrow;
    }
}

static int lmpn_divmod_2n_n(limb_t *q, limb_t *a, size_t n, const limb_t *b, size_t m) {
    //printf("%d %d\n", n, m);
    if (n-m < DIV_DC_THRESHOLD) {
        lmpn_divmod_knuth(q, a, n, b, m);
        return 0;
    }
    size_t lo = m>>1, hi = m-lo;
    assert(3*m/2 < n);
    assert(n <= 2*m);
    assert(m+hi <= n);
    assert(hi >= lo);
    assert(b[m-1] >> (LIMB_WIDTH-1));
    assert(b[m-1] >= a[n-1]);
    if (LMP_UNLIKELY(lmpn_divmod_3n_2n(q+hi, a+hi, n-hi, b, m))) return 1;
    if (m & 1) {
        // i-m = hi-1, i=m+hi-1
        q[hi-1] = limb_div(a[m+hi-1], a[m+hi-2], b[m-1]);
        limb_t borrow = lmpn_submul_1(a+hi-1, b, m, q[hi-1]);
        while (borrow > a[m+hi-1]) {
            --q[hi-1];
            borrow -= lmpn_add_n(a+hi-1, a+hi-1, b, m);
        }
        a[m+hi-1] -= borrow;
    }
    return lmpn_divmod_3n_2n(q, a, m+lo, b, m);
}

static int lmpn_divmod_3n_2n(limb_t *q, limb_t *a, size_t n, const limb_t *b, size_t m) {
    if (n-m < DIV_DC_THRESHOLD) {
        lmpn_divmod_knuth(q, a, n, b, m);
        return 0;
    }
    size_t lo = m>>1, hi = m-lo;
    assert(m < n);
    assert(n <= (3*m)/2);
    assert(hi >= lo);
    assert((b[m-1] >> (LIMB_WIDTH-1)));
    assert(b[m-1] >= a[n-1]);

    assert(lmpn_cmp(a+hi+lo, b+lo, hi) == -1);
    limb_t k = n-m; // n>=2*k = 2n-2m -> n - 2m <= 0, m >= k -> m>=n-m -> 2m>=n
    // n-m >= 2m-n -> 2n >= 3m -> n >= 1.5m
    if (LMP_UNLIKELY(lmpn_divmod_2n_n(q, a+n-2*k, 2*k, b+m-k, k))) return 1;
    limb_t *d = lmpn_alloc(m);
    if (LMP_UNLIKELY(!d)) return 1;
    if (LMP_UNLIKELY(lmpn_mul(d, b, m-k, q, n-m))) {free(d); return 1;}
    int borrow = lmpn_sub(a, a, n, d, m);
    free(d);

    while (borrow) {
        lmpn_sub_1(q, q, n-m, 1);
        borrow -= lmpn_add(a, a, n, b, m);
    }

    assert(lmpn_cmp(a, b, m) < 0);
    return 0;
}

static int lmpn_divmod_dc(limb_t *q, limb_t *a, size_t n, const limb_t *b, size_t m) {
    while (n > 2*m) {
        n -= m;
        if (LMP_UNLIKELY(lmpn_divmod_2n_n(q+n-m, a+n-m, 2*m, b, m))) return 1;
    }
    if (n > 3*m/2) return lmpn_divmod_2n_n(q, a, n, b, m);
    else if (n - m >= DIV_DC_THRESHOLD) return lmpn_divmod_3n_2n(q, a, n, b, m);
    else if (n >= m) {lmpn_divmod_knuth(q, a, n, b, m); return 0;}
    else return 0;
}

int _lmpn_div_qr(limb_t *q, limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(b[m-1] >> (LIMB_WIDTH-1));
    if (n-m < DIV_DC_THRESHOLD) {lmpn_divmod_knuth(q, a, n, b, m); return 0;}
    else return lmpn_divmod_dc(q, a, n, b, m);
}

int _lmpn_div_r(limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(b[m-1] >> (LIMB_WIDTH-1));
    if (n-m < DIV_DC_THRESHOLD) {
        for (size_t i=n-1;i>=m;--i) {
            limb_t borrow = lmpn_submul_1(a+i-m, b, m, limb_div(a[i], a[i-1], b[m-1]));
            while (borrow > a[i]) borrow -= lmpn_add_n(a+i-m, a+i-m, b, m);
            a[i] -= borrow;
        }
        return 0;
    } else {
        limb_t *q = lmpn_alloc(n-m);
        if (LMP_UNLIKELY(!q)) return 1;
        int r = lmpn_divmod_dc(q, a, n, b, m);
        free(q);
        return r;
    }
}