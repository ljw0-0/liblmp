#include "lmp.h"
#include "impl.h"

limb_t limb_mul_mod(limb_t *restrict hi, limb_t *restrict lo, limb_t a, limb_t b, limb_t m) {
    #ifdef tmp_t
    tmp_t t = (tmp_t)a * (tmp_t)b;
    *lo = (limb_t)t;
    *hi = (limb_t)(t >> LIMB_WIDTH);
    return t % m;
    #else
    # error "Unsupported limb."
    #endif
}