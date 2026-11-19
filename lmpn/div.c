#include "lmp.h"
#include <string.h>

static int generic(limb_t *q, limb_t *r, int cdiv, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m && m >= 1);
    int clz = limb_clz(b[m-1]);
    if (clz) {
        limb_t *tmp_a = lmpn_alloc(n);
        if (LMP_UNLIKELY(!tmp_a)) return 1;
        limb_t *tmp_b = lmpn_alloc(m), *tp = tmp_a+n-m;
        if (LMP_UNLIKELY(!tmp_b)) {cleanup: free(tmp_a); return 1;}
        limb_t hi = lmpn_lshift(tmp_a, a, n, clz);
        lmpn_lshift(tmp_b, b, m, clz);

        if (q) {
            limb_t qh = limb_div(hi, tmp_a[n-1], tmp_b[m-1]);
            limb_t borrow = lmpn_submul_1(tp, tmp_b, m, qh);
            while (borrow > hi) {
                --qh;
                borrow -= lmpn_add_n(tp, tp, tmp_b, m);
            }
            q[n-m] = qh;
            if (LMP_UNLIKELY(n > m && _lmpn_div_qr(q, tmp_a, n, tmp_b, m))) {free(tmp_b); goto cleanup;}
        } else {
            limb_t borrow = lmpn_submul_1(tp, tmp_b, m, limb_div(hi, tmp_a[n-1], tmp_b[m-1]));
            while (borrow > hi) borrow -= lmpn_add_n(tp, tp, tmp_b, m);
            if (LMP_UNLIKELY(n > m && _lmpn_div_r(tmp_a, n, tmp_b, m))) {free(tmp_b); goto cleanup;}
        }

        if (cdiv && !lmpn_is_zero(tmp_a, m)) {
            if (q) lmpn_add_1(q, q, n-m+1, 1);
            if (r) lmpn_subrsh_n(r, b, tmp_a, m, clz);
        } else if (r) lmpn_rshift(r, tmp_a, m, clz);

        free(tmp_a);
        free(tmp_b);
    } else {
        limb_t *tmp = lmpn_alloc(n);
        if (LMP_UNLIKELY(!tmp)) return 1;
        memcpy(tmp, a, n*sizeof(limb_t));
        if (tmp[n-1] >= b[m-1]) {
            limb_t *tp = tmp+n-m;
            if (q) {
                limb_t qh = tmp[n-1] / b[m-1];
                limb_t borrow = lmpn_submul_1(tp, b, m, qh);
                while (borrow) {
                    --qh;
                    borrow -= lmpn_add_n(tp, tp, b, m);
                }
                q[n-m] = qh;
            } else {
                limb_t borrow = lmpn_submul_1(tp, b, m, tmp[n-1] / b[m-1]);
                while (borrow) borrow -= lmpn_add_n(tp, tp, b, m);
            }
        } else if (q) q[n-m] = 0;
        if (LMP_UNLIKELY(n > m && (q ? _lmpn_div_qr(q, tmp, n, b, m) : _lmpn_div_r(tmp, n, b, m)))) {free(tmp); return 1;}
        if (cdiv && !lmpn_is_zero(tmp, m)) {
            if (q) lmpn_add_1(q, q, n-m+1, 1);
            if (r) lmpn_sub_n(r, b, tmp, m);
        } else if (r) memcpy(r, tmp, m*sizeof(limb_t));
        free(tmp);
    }
    return 0;
}


int lmpn_div_qr(limb_t *q, limb_t *r, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    return generic(q, r, 0, a, n, b, m);
}

int lmpn_div_q(limb_t *q, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    return generic(q, NULL, 0, a, n, b, m);
}

int lmpn_div_r(limb_t *r, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    return generic(NULL, r, 0, a, n, b, m);
}

int lmpn_cdiv_q(limb_t *q, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    return generic(q, NULL, 1, a, n, b, m);
}

int lmpn_cdiv_r(limb_t *r, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    return generic(NULL, r, 1, a, n, b, m);
}

int lmpn_cdiv_qr(limb_t *q, limb_t *r, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    return generic(q, r, 1, a, n, b, m);
}
