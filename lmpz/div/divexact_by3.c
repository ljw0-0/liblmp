#include "lmp.h"

int lmpz_divexact_by3(lmpz_t q, const lmpz_t a) {
    if (lmpz_size(a) <= 1) lmpz_set_signed_ui(q, lmpz_get_ui(a) * limb_inv3, lmpz_sign_bit(a));
    else {
        int err = lmpz_reserve(q, lmpz_size(a));
        if (LMP_UNLIKELY(err)) return err;
        lmpn_divexact_by3(lmpz_ptr(q), lmpz_ptr(a), lmpz_size(a));
        q->size = lmpz_sign_bit(a) | (lmpz_size(a) - !lmpz_at(q, lmpz_size(a)-1));
    }
    return 0;
}