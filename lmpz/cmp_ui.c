#include "lmp.h"

int lmpz_cmp_ui(const lmpz_t a, limb_t b) {
    if (lmpz_sgn_bit(a)) return -1;
    if (lmpz_cap(a)) {
        if (lmpz_size(a) > 1) return 1;
        else return limb_cmp(lmpz_at(a, 0), b);
    } else return limb_cmp(a->limb, b);
}