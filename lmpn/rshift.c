#include "lmp.h"

limb_t lmpn_rshift(limb_t *b, const limb_t *a, size_t n, int cnt) {
    limb_t ret = (a[0] & (((limb_t)1 << cnt) - 1));

    while (--n) {
        *b = (*a >> cnt) | (a[1] << (LIMB_WIDTH - cnt));
        ++a;
        ++b;
    }
    
    *b = *a >> cnt;
    return ret;
}