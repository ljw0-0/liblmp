#include "lmp.h"
#include "impl.h"
#include <stdlib.h>

int lmpz_reserve(lmpz_t z, size_t n) {
    unlikely (n > MPN_MAX_LEN) return lmpz_EOVERFLOW;
    if (LMP_LIKELY(lmpz_cap(z)) < n) {
        void *p = lmpz_cap(z) ? mpn_realloc(lmpz_ptr(z), n) : mpn_alloc(n);
        if (LMP_UNLIKELY(!p)) return lmpz_ENOMEM;
        z->limb = (uintptr_t)p;
        z->alloc = n;
    }
    return 0;
}