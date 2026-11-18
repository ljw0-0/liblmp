#include "lmp.h"

void lmpn_ntt_dit(limb_t *restrict a, size_t n, limb_t p, const limb_t *restrict wki) {
    assert(n > 0 && (n & (n - 1)) == 0); // n must be a power of 2
    assert(p <= LIMB_MAX / 2);
    assert(*wki == p-1);

    for (size_t len=2;len<=n;len<<=1) {
        // DIT is the inverse transform, so use the inverse DIF twiddle.
        limb_t w = *wki, r = limb_div(w, 0, p);
        for (size_t i=0;i<n;i+=len) {
            limb_t cur = 1;
            for (size_t j=i,mx=i+(len>>1);j<mx;++j) {
                limb_t u = a[j], v = limb_mulm(a[j+len/2], cur, p);

                a[j] = u + v;
                if (a[j] >= p) a[j] -= p;
                a[j+(len>>1)] = (u < v ? u-v+p : u-v);

                cur = cur * w - limb_mul_hi(cur, r) * p;
                if (cur >= p) cur -= p;
            }
        }
        ++wki;
    }

    limb_t invn = limb_invm(n, p), r = limb_div(invn, 0, p);
    for (size_t i=0;i<n;++i) {
        a[i] = a[i] * invn - limb_mul_hi(a[i], r) * p;
        if (a[i] >= p) a[i] -= p;
    }
}