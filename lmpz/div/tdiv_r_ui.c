#include "lmp.h"
#include "impl.h"

limb_t lmpz_tdiv_r_ui(const lmpz_t a, limb_t b) {
    if (LMP_UNLIKELY(b==0)) return (limb_t)(-1);
    if (lmpz_size(a) <= 1) return lmpz_get_ui(a) % b;
    return lmpn_div_r_1(lmpz_ptr(a), lmpz_size(a), b);
}