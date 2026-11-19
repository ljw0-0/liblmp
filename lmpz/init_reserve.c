#include "lmp.h"
#include "impl.h"
#include <stdlib.h>

int lmpz_init_reserve(lmpz_t z, size_t n) {
    unlikely (n > MPN_MAX_LEN) return lmpz_EOVERFLOW;
    if (LMP_LIKELY(n)) {
        void *p = mpn_alloc(n);
        if (LMP_UNLIKELY(!p)) return lmpz_ENOMEM;
        z->alloc = n;
        z->limb = (uintptr_t)p;
    } else lmpz_init(z);
    return 0;
}