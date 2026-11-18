#include "lmp.h"

static void bdiv_q_basecase(limb_t *q, limb_t *a, size_t n, const limb_t *b, size_t m) {
    limb_t inv = limb_inv(b[0]);
    while (n > m) {
        lmpn_sub_1(a+m, a+m, n-m, lmpn_submul_1(a, b, m, *q=inv*(*a)));
        ++a; ++q;
        --n;
    }
    do {
        lmpn_submul_1(a, b, n, *q=inv*(*a));
        ++a; ++q;
    } while (--n);
}

int _lmpn_bdiv_q(limb_t *q, limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    bdiv_q_basecase(q, a, n, b, m);
    return 0;
}
