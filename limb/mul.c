#include "lmp.h"
#include "impl.h"

#ifdef tmp_t
void limb_mul(limb_t *restrict hi, limb_t *restrict lo, limb_t a, limb_t b) {
    tmp_t t = (tmp_t)a * (tmp_t)b;
    *lo = (limb_t)t;
    *hi = (limb_t)(t >> LIMB_WIDTH);
}
#elif 
#else
# error "Unsupported limb."
#endif