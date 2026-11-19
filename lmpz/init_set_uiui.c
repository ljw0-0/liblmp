#include "lmp.h"
#include "impl.h"

int lmpz_init_set_uiui(lmpz_t z, limb_t hi, limb_t lo) {
    if (hi == 0) lmpz_init_set_ui(z, lo);
    else {
        int r = lmpz_init_reserve(z, 2);
        if (LMP_UNLIKELY(r)) return r;
        lmpz_at(z, 0) = lo;
        lmpz_at(z, 1) = hi;
        z->size = 2;
    }
    return 0;
}