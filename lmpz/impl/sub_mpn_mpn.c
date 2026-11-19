#include "lmp.h"
#include "impl.h"

int _lmpz_sub_mpn_mpn(lmpz_t z, size_t sign_bit, const limb_t *ap, size_t an, const limb_t *bp, size_t bn) {
    int r = lmpz_reserve(z, an);
    if (LMP_UNLIKELY(r)) return r;
    mpn_sub(lmpz_ptr(z), ap, an, bp, bn);
    z->size = mpn_norm(lmpz_ptr(z), an) | sign_bit;
    return 0;
}