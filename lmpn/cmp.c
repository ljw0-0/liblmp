#include "lmp.h"

int lmpn_cmp(const limb_t *a, const limb_t *b, size_t n) {
    a += n-1;
    b += n-1;
    while (--n) {
        limb_t x = *a, y = *b;
        if (x ^ y) return limb_cmp(x, y);
        --a;
        --b;
    }
    return limb_cmp(*a, *b);
}
