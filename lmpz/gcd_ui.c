#include "lmp.h"

limb_t lmpz_gcd_ui(const lmpz_t a, limb_t b) {
    return limb_gcd(lmpz_size(a) <= 1 ? lmpz_get_ui(a) : lmpn_div_r_1(lmpz_ptr(a), lmpz_size(a), b), b);
}