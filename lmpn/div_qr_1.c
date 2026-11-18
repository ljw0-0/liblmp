#include "lmp.h"

limb_t lmpn_div_qr_1(limb_t *q, const limb_t *a, size_t n, limb_t b) {
    limb_t hi=0;
    a += n;
    q += n;
    do {
        if (hi) hi = limb_divmod(--q, hi, *(--a), b);
        else {
            hi = *(--a) % b;
            *(--q) = *a / b;
        }
    } while (--n);
    return hi;
}