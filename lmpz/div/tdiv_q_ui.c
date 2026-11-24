#include "lmp.h"

int lmpz_tdiv_q_ui(lmpz_t q, const lmpz_t a, limb_t b) {
    if (LMP_UNLIKELY(!b)) return LMP_EDIVZERO;
    if (lmpz_is_zero(a)) lmpz_zero(q);
    else if (lmpz_size(a) == 1) lmpz_set_signed_ui(q, lmpz_get_ui(a) / b, lmpz_sign_bit(a));
    else {
        int err = lmpz_reserve(q, lmpz_size(a));
        if (LMP_UNLIKELY(err)) return err;
        lmpn_div_qr_1(lmpz_ptr(q), lmpz_ptr(a), lmpz_size(a), b);
        q->size = lmpz_gen_size(lmpz_size(a) - !lmpz_at(q, lmpz_size(a)-1), lmpz_sign_bit(a));
    }
    return 0;
}