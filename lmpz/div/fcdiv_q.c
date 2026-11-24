#include "lmp.h"

static int lmpz_fcdiv_q(lmpz_t q, const lmpz_t a, const lmpz_t b, size_t sign_bit) {
    int err;
    if (LMP_UNLIKELY(lmpz_is_zero(b))) return LMP_EDIVZERO;
    size_t sa = lmpz_size(a), sb = lmpz_size(b);
    size_t sign_a = lmpz_sign_bit(a), sign_b = lmpz_sign_bit(b);
    int cdiv = ((sign_a ^ sign_b ^ sign_bit) & LMPZ_SIGN_BIT) != 0;

    if (lmpz_is_zero(a)) {
        lmpz_zero(q);
    } else if (sa < sb) {
        if (!cdiv) lmpz_zero(q);
        else lmpz_set_signed_ui(q, 1, sign_a ^ sign_b);
    } else if (sa == 1) {
        limb_t au = lmpz_get_ui(a), bu = lmpz_get_ui(b);
        lmpz_set_signed_ui(q, au / bu + (cdiv && au % bu), sign_a ^ sign_b);
    } else if (sb == 1) {
        if (LMP_UNLIKELY(err = lmpz_reserve(q, sa))) return err;
        if (lmpn_div_qr_1(lmpz_ptr(q), lmpz_ptr(a), sa, lmpz_get_ui(b)) && cdiv) {
            #ifdef NDEBUG
            lmpn_add_1(lmpz_ptr(q), lmpz_ptr(q), sa, 1);
            #else
            limb_t carry = lmpn_add_1(lmpz_ptr(q), lmpz_ptr(q), sa, 1);
            assert(!carry);
            #endif
        }
        q->size = lmpz_gen_size(sa - !lmpz_at(q, sa-1), sign_a ^ sign_b);
    } else {
        size_t qn = sa - sb + 1;
        if (LMP_UNLIKELY((err = lmpz_reserve(q, qn)) || (err = cdiv ? lmpn_cdiv_q(lmpz_ptr(q), lmpz_ptr(a), sa, lmpz_ptr(b), sb) : lmpn_div_q(lmpz_ptr(q), lmpz_ptr(a), sa, lmpz_ptr(b), sb)))) return err;
        q->size = lmpz_gen_size(qn - !lmpz_at(q, qn-1), sign_a ^ sign_b);
    }
    return 0;
}

int lmpz_fdiv_q(lmpz_t q, const lmpz_t a, const lmpz_t b) {return lmpz_fcdiv_q(q, a, b, 0);}
int lmpz_cdiv_q(lmpz_t q, const lmpz_t a, const lmpz_t b) {return lmpz_fcdiv_q(q, a, b, LMPZ_SIGN_BIT);}