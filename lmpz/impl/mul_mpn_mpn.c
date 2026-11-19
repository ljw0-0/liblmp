#include "lmp.h"
#include "impl.h"

int _lmpz_mul_mpn_mpn(lmpz_t z, size_t sign_bit, limb_t *ap, size_t an, limb_t *bp, size_t bn) {
    assert(ap[an-1]);
    assert(bp[bn-1]);

    int r = lmpz_reserve(z, an+bn);
    if (LMP_UNLIKELY(r)) return r;

    r = an >= bn ? lmpn_mul(lmpz_ptr(z), ap, an, bp, bn) : lmpn_mul(lmpz_ptr(z), bp, bn, ap, an);
    if (LMP_UNLIKELY(r)) return r;

    z->size = (an+bn-!lmpz_at(z, an+bn-1))|sign_bit;

    return 0;
}