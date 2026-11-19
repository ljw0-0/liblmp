#include "lmp.h"
#include "impl.h"

int _lmpz_add_mpn_mpn(lmpz_t z, size_t sign_bit, const limb_t *ap, size_t an, const limb_t *bp, size_t bn) {
    int r = lmpz_reserve(z, an+1);
    if (LMP_UNLIKELY(r)) return r;
    z->size = (lmpn_add(lmpz_ptr(z), ap, an, bp, bn) + an) | sign_bit;
    return 0;
}