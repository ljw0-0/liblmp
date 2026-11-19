#include "lmp.h"
#include "impl.h"
#include <string.h>

int lmpz_init_neg(lmpz_t z, const lmpz_t a) {
    if (lmpz_cap(a)) {
        int r = lmpz_init_reserve(z, lmpz_size(a));
        if (LMP_UNLIKELY(r)) return r;
        memcpy(lmpz_ptr(z), lmpz_ptr(a), lmpz_size(a)*sizeof(limb_t));
    } else z->limb = a->limb;
    z->size = lmpz_neg_size(a->size);
    return 0;
}