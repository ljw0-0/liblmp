#include "lmp.h"

static int lmpz_fcdiv_q_ui(lmpz_t q, const lmpz_t a, limb_t b, size_t sign_bit) {
    if (LMP_UNLIKELY(!b)) return LMP_EDIVZERO;
    if (lmpz_is_zero(a)) lmpz_zero(q);
    else if (a->size == (1 ^ sign_bit)) lmpz_set_signed_ui(q, lmpz_get_ui(a)/b, lmpz_sign_bit(a));
    else if (lmpz_size(a) == 1) lmpz_set_signed_ui(q, (lmpz_get_ui(a)-1)/b+1, lmpz_sign_bit(a));
    else {
        int err = lmpz_reserve(q, lmpz_size(a));
        if (LMP_UNLIKELY(err)) return err;
        if (lmpn_div_qr_1(lmpz_ptr(q), lmpz_ptr(a), lmpz_size(a), b) && (lmpz_sign_bit(a) ^ sign_bit)) lmpn_add_1(lmpz_ptr(q), lmpz_ptr(q), lmpz_size(a), 1);
        q->size = lmpz_gen_size(lmpz_size(a) - !lmpz_at(q, lmpz_size(a)-1), lmpz_sign_bit(a));
    }
    return 0;
}

int lmpz_fdiv_q_ui(lmpz_t q, const lmpz_t a, limb_t b) {return lmpz_fcdiv_q_ui(q, a, b, 0);}
int lmpz_cdiv_q_ui(lmpz_t q, const lmpz_t a, limb_t b) {return lmpz_fcdiv_q_ui(q, a, b, LMPZ_SIGN_BIT);}