#include "lmp.h"

int lmpz_cmp(const lmpz_t a, const lmpz_t b) {
    if (a == b) return 0;
    if (a->size == b->size) {
        if (lmpz_sgn_bit(a)) {const lmpz *tmp = a; a = b; b = tmp;}
        if (lmpz_cap(a)) {
            size_t n = lmpz_size(a);
            if (n == 0) return 0;
            if (n == 1) return limb_cmp(lmpz_at(a, 0), lmpz_at(b, 0)); 
            return lmpn_cmp(lmpz_ptr(a), lmpz_ptr(b), n-1);
        }
        return limb_cmp(a->limb, b->limb);
    } else {
        size_t msk, sa, sb;
        #ifdef SIZE_WIDTH
        # define get(s) msk = -(size_t)((s) >> (LIMB_WIDTH - 1));
        #elif defined(__SIZE_WIDTH__)
        # define get(s) msk = -(size_t)((s) >> (__SIZE_WIDTH__ - 1));
        #else
        # define get(s) msk = -(size_t)((s) > (LIMB_WIDTH >> 1));
        #endif
        get(a->size)
        sa = (a->size ^ msk) + ((SIZE_MAX >> 1) & ~msk);
        get(b->size)
        sb = (b->size ^ msk) + ((SIZE_MAX >> 1) & ~msk);
        return (sa > sb) - (sa < sb);
    }
}