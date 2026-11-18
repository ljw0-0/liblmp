#include "lmp.h"

limb_t lmpn_lshift(limb_t *b, const limb_t *a, size_t n, int cnt) {
    limb_t ret;
    a += n-1;
    b += n-1;
    ret = *a >> (LIMB_WIDTH - cnt);
    while (--n) {
        *b = (*a << cnt) | (a[-1] >> (LIMB_WIDTH - cnt));
        --a;
        --b;
    }
    *b = *a << cnt;
    return ret;
}