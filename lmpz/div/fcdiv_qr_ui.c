#include "lmp.h"

static limb_t lmpz_fcdiv_qr_ui(lmpz_t q, const lmpz_t a, limb_t b, size_t sign_bit) {
    if (LMP_UNLIKELY(!b)) return (limb_t)(-1);
    if (lmpz_is_zero(a)) {
        lmpz_zero(q);
        return 0;
    }
    if (a->size == (1 ^ sign_bit)) {
        limb_t ui = lmpz_get_ui(a);
        lmpz_set_signed_ui(q, ui/b, lmpz_sgn_bit(a));
        return ui%b;
    }
    if (lmpz_size(a) == 1) {
        limb_t ui = lmpz_get_ui(a), r = ui%b;
        lmpz_set_signed_ui(q, (ui-1)/b+1, lmpz_sgn_bit(a));
        return r ? b-r : 0;
    }
    if (LMP_UNLIKELY(lmpz_reserve(q, lmpz_size(a)))) return (limb_t)(-1);
    limb_t r = lmpn_div_qr_1(lmpz_ptr(q), lmpz_ptr(a), lmpz_size(a), b);
    if ((lmpz_sgn_bit(a) ^ sign_bit) && r) {
        lmpn_add_1(lmpz_ptr(q), lmpz_ptr(q), lmpz_size(a), 1);
        r = b-r;
    }
    q->size = lmpz_sgn_bit(a) | (lmpz_size(a) - !lmpz_at(q, lmpz_size(a)-1));
    return r;
}

limb_t lmpz_fdiv_qr_ui(lmpz_t q, const lmpz_t a, limb_t b) {return lmpz_fcdiv_qr_ui(q, a, b, 0);}
limb_t lmpz_cdiv_qr_ui(lmpz_t q, const lmpz_t a, limb_t b) {return lmpz_fcdiv_qr_ui(q, a, b, LMPZ_SIGN_BIT);}