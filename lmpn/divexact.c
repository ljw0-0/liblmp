#include "lmp.h"
#include <string.h>

int lmpn_divexact(limb_t *q, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    limb_t *ta;
    int r;

    assert(n >= m && m > 0);
    while (m && !*b) {++b; ++a; --n; --m;}

    assert(m);
    size_t b_len = m;
    
    if (m==1) {
        lmpn_divexact_1(q, a, n, *b);
        return 0;
    }

    if (LMP_UNLIKELY(!(ta = lmpn_alloc((n -= m-1))))) return 1;
    if (n < m) m = n;

    if (b[0] & 1) {
        memcpy(ta, a, n*sizeof(limb_t));
        r = _lmpn_bdiv_q(q, ta, n, b, m);
    } else {
        limb_t *tb = lmpn_alloc(m);
        int ctz;
        if (LMP_UNLIKELY(!tb)) {free(ta); return 1;}
        lmpn_rshift(ta, a, n, ctz = limb_ctz(b[0]));
        ta[n-1] |= a[n] << (LIMB_WIDTH - ctz);
        lmpn_rshift(tb, b, m, ctz);
        if (m < b_len) tb[m-1] |= b[m] << (LIMB_WIDTH - ctz);
        r = _lmpn_bdiv_q(q, ta, n, tb, m);
    }

    free(ta);
    return r;
}
