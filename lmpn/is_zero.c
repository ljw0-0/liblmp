#include "lmp.h"

int lmpn_is_zero(const limb_t *a, size_t n) {
    do {
        if (*a) return 0;
        ++a;
    } while(--n);
    return 1;
}