#include "lmp.h"

int lmpz_equal_si(const lmpz_t a, slimb_t b) {
    if (lmpz_size(a) > 1) return 0;
    if (lmpz_sgn_bit(a)) return b < 0 && -(limb_t)b == lmpz_get_ui(a);
    return b >= 0 && lmpz_get_ui(a) == (limb_t)b;
}