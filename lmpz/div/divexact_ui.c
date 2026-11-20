#include "lmp.h"

int lmpz_divexact_ui(lmpz_t q, const lmpz_t a, limb_t b) {
    if (lmpz_size(a) <= 1) {
        int ctz = limb_ctz(b);
        lmpz_set_signed_ui(q, (lmpz_get_ui(a) >> ctz) * limb_inv(b >> ctz), lmpz_sgn_bit(a));
    } else {
        int err = lmpz_reserve(q, lmpz_size(a));
        if (LMP_UNLIKELY(err)) return err;
        lmpn_divexact_1(lmpz_ptr(q), lmpz_ptr(a), lmpz_size(a), b);
        q->size = lmpz_sgn_bit(a) | (lmpz_size(a) - !lmpz_at(q, lmpz_size(a)-1));
    }
    return 0;
}