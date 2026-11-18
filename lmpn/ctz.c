#include "lmp.h"

size_t lmpn_ctz(const limb_t *a, size_t n) {
    for (size_t i=0;i<n;++i) {
        if (*a) return i*LIMB_WIDTH + limb_ctz(*a);
        ++a;
    }
}