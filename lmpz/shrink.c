#include "lmp.h"
#include <stdlib.h>

void lmpz_shrink(lmpz_t z) {
    if (LMP_LIKELY(lmpz_size(z) < lmpz_cap(z))) {
        if (lmpz_size(z) <= 1) {
            limb_t val = lmpz_at(z, 0);
            free(lmpz_ptr(z));
            z->alloc = 0;
            z->limb = val;
        } else {
            limb_t *p = lmpn_realloc(lmpz_ptr(z), lmpz_size(z));
            z->alloc = lmpz_size(z);
            if (LMP_LIKELY(p)) z->limb = (uintptr_t)p;
        }
    }
}