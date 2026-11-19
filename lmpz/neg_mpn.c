#include "lmp.h"
#include "impl.h"
#include "string.h"

int lmpz_neg_mpn(lmpz_t z, const limb_t *ap, size_t an) {
    int r = lmpz_reserve(z, an);
    if (LMP_UNLIKELY(r)) return r;
    memcpy(lmpz_ptr(z), ap, an*sizeof(limb_t));
    z->size = lmpz_neg_size(mpn_norm(ap, an));
    return 0;
}