#include "lmp.h"
#include "impl.h"

limb_t lmpz_cdiv_r_ui(const lmpz_t a, limb_t b) {
    if (LMP_UNLIKELY(b==0)) return (limb_t)(-1);
    limb_t r = lmpz_size(a) <= 1 ? lmpz_get_ui(a) % b : lmpn_div_r_1(lmpz_ptr(a), lmpz_size(a), b);
    return r ? b-r : 0;
}