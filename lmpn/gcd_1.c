#include "lmp.h"

limb_t lmpn_gcd_1(const limb_t *a, size_t n, limb_t b) {
    assert(b);
    return limb_gcd(lmpn_div_r_1(a, n, b), b);
}