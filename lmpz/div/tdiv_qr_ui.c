#include "lmp.h"

limb_t lmpz_tdiv_qr_ui(lmpz_t q, const lmpz_t a, limb_t b) {
    if (LMP_UNLIKELY(!b)) return (limb_t)(-1);
    if (lmpz_is_zero(a)) {
        lmpz_zero(q);
        return 0;
    }
    if (lmpz_size(a) == 1) {
        limb_t ui = lmpz_get_ui(a);
        lmpz_set_signed_ui(q, ui / b, lmpz_sign_bit(a));
        return ui % b;
    }
    if (LMP_UNLIKELY(lmpz_reserve(q, lmpz_size(a)))) return (limb_t)(-1);
    limb_t r = lmpn_div_qr_1(lmpz_ptr(q), lmpz_ptr(a), lmpz_size(a), b);
    q->size = lmpz_gen_size(lmpz_size(a) - !lmpz_at(q, lmpz_size(a)-1), lmpz_sign_bit(a));
    return r;
}