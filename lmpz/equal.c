#include "lmp.h"
#include <string.h>

int lmpz_equal(const lmpz_t a, const lmpz_t b) {
    if (a == b) return 1;
    if (a->size ^ b->size) return 0;
    if (a->alloc) {
        if (b->alloc) return !memcmp(lmpz_ptr(a), lmpz_ptr(b), lmpz_size(a)*sizeof(limb_t));
        return lmpz_at(a, 0) == lmpz_limb(b);
    }
    if (b->alloc) return lmpz_limb(a) == lmpz_at(b, 0);
    return lmpz_limb(a) == lmpz_limb(b);
}