#include "lmp.h"

size_t lmpn_clz(const limb_t *a, size_t n) {
    a += n-1;
    for (size_t i=0;i<n;++i) {
        if (*a) return i*LIMB_WIDTH + limb_clz(*a);
        --a;
    }
}