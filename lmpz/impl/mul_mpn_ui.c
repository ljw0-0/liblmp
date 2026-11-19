#include "lmp.h"
#include "impl.h"

int _lmpz_mul_mpn_ui(lmpz_t z, size_t sign_bit, limb_t *ap, size_t an, limb_t b) {
    assert(ap[an-1]);

    int r = lmpz_reserve(z, an+1);
    if (LMP_UNLIKELY(r)) return r;
    lmpz_at(z, an) = mpn_mul_1(lmpz_ptr(z), ap, an, b);
    z->size = (an + !!lmpz_at(z, an)) | sign_bit;
    return 0;
}