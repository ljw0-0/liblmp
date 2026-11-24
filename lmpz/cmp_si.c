#include "lmp.h"

int lmpz_cmp_si(const lmpz_t a, slimb_t b) {
    if (lmpz_sign_bit(a)) {
        if (b > 0) return -1;
        if (lmpz_cap(a)) {
            if (lmpz_size(a) > 1) return -1;
            return limb_cmp((limb_t)(-b), lmpz_at(a, 0));
        }
        return limb_cmp((limb_t)(-b), a->limb);
    } else {
        if (b < 0) return 1;
        if (lmpz_cap(a)) {
            if (lmpz_size(a) > 1) return 1;
            else return limb_cmp(lmpz_at(a, 0), (limb_t)b);
        } else return limb_cmp(a->limb, (limb_t)b);
    }
}