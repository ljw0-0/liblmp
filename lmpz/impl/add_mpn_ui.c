#include "lmp.h"
#include "impl.h"

int _lmpz_add_mpn_ui(lmpz_t z, size_t sign_bit, const limb_t *ap, size_t an, limb_t b) {
    int r = lmpz_reserve(z, an+1);
    if (LMP_UNLIKELY(r)) return r;
    z->size = (lmpn_add_1(lmpz_ptr(z), ap, an, b) + an) | sign_bit;
    return 0;
}