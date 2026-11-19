#include "lmp.h"
#include "impl.h"
#include <string.h>

int lmpz_set(lmpz_t z, const lmpz_t a) {
    likely (z != a) {
        if (!lmpz_cap(a) && !lmpz_cap(z)) z->limb = a->limb;
        else {
            int r = lmpz_reserve(z, lmpz_size(a));
            if (LMP_UNLIKELY(r)) return r;
            memcpy(lmpz_ptr(z), lmpz_get_mpn(a), lmpz_size(a)*sizeof(limb_t));
        }
        z->size = a->size;
    }
    return 0;
}