#include "lmp.h"
#include <stdlib.h>

int lmpz_init_reserve(lmpz_t z, size_t n) {
    if (LMP_UNLIKELY(n > LMPN_MAX_LEN)) return LMP_EOVERFLOW;
    if (LMP_LIKELY(n)) {
        void *p = lmpn_alloc(n);
        if (LMP_UNLIKELY(!p)) return LMP_ENOMEM;
        z->alloc = n;
        z->limb = (uintptr_t)p;
    } else lmpz_init(z);
    return 0;
}