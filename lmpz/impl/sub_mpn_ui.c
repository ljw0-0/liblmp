#include "lmp.h"
#include "impl.h"

int _lmpz_sub_mpn_ui(lmpz_t z, size_t sign_bit, const limb_t *ap, size_t an, limb_t b) {
    int r = lmpz_reserve(z, an);
    if (LMP_UNLIKELY(r)) return r;
    z->size = (an - lmpn_sub_1(lmpz_ptr(z), ap, an, b)) | sign_bit;
    return 0;
}