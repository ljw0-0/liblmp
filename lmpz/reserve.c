#include "lmp.h"
#include <stdlib.h>

int lmpz_reserve(lmpz_t z, size_t n) {
    if (LMP_UNLIKELY(n > LMPN_MAX_LEN)) return LMP_EOVERFLOW;
    if (LMP_LIKELY(lmpz_cap(z)) < n) {
        void *p = lmpz_cap(z) ? lmpn_realloc(lmpz_ptr(z), n) : lmpn_alloc(n);
        if (LMP_UNLIKELY(!p)) return LMP_ENOMEM;
        z->limb = (uintptr_t)p;
        z->alloc = n;
    }
    return 0;
}