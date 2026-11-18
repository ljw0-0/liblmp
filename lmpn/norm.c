#include "lmp.h"

size_t lmpn_norm(const limb_t *a, size_t n) {
    while (n && !a[n-1]) --n;
    return n;
}