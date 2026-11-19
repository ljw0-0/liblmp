#include "lmp.h"
#include "impl.h"

int lmpz_mpn_sub(lmpz_t z, const limb_t *ap, size_t an, const lmpz_t b) {
    if (lmpz_sgn_bit(b)) return lmpz_add_mpn(z, b, ap, an);
    if (b->size > an) return _lmpz_sub_mpn_mpn(z, lmpz_SIGN_BIT, lmpz_limbs(b), b->size, ap, an);
    if (b->size < an) return _lmpz_sub_mpn_mpn(z, 0, ap, an, lmpz_limbs(b), b->size);
    int r = lmpz_reserve(z, an);
    if (LMP_UNLIKELY(r)) return r;
    if (mpn_sub_n(lmpz_ptr(z), ap, lmpz_get_mpn(z), an)) {
        mpn_neg(lmpz_ptr(z), lmpz_ptr(z), an);
        z->size = mpn_norm(lmpz_ptr(z), an) | lmpz_SIGN_BIT;
    } else z->size = mpn_norm(lmpz_ptr(z), an);
    return 0;
}