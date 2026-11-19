#include "lmp.h"
#include "impl.h"

int lmpz_set_signed_uiui(lmpz_t z, limb_t hi, limb_t lo, size_t sign_bit) {
    if (hi == 0) lmpz_set_signed_ui(z, lo, sign_bit);
    else {
        int r = lmpz_reserve(z, 2);
        if (LMP_UNLIKELY(r)) return r;
        lmpz_at(z, 0) = lo;
        lmpz_at(z, 1) = hi;
        z->size = 2 ^ sign_bit;
    }
    return 0;
}