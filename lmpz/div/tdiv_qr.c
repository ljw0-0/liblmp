#include "lmp.h"

int lmpz_tdiv_qr(lmpz_t q, lmpz_t r, const lmpz_t a, const lmpz_t b) {
    int err;
    if (LMP_UNLIKELY(lmpz_is_zero(b))) return LMP_EDIVZERO;
    if (lmpz_is_zero(a)) {
        lmpz_zero(q);
        lmpz_zero(r);
    } else if (lmpz_size(a) < lmpz_size(b)) {
        if (LMP_UNLIKELY(err = lmpz_set(r, a))) return err;
        lmpz_zero(q);
    } else if (lmpz_size(a) == 1) {
        limb_t au = lmpz_get_ui(a), bu = lmpz_get_ui(b);
        assert(lmpz_size(b) == 1);
        lmpz_set_signed_ui(r, au % bu, a->size ^ 1);
        lmpz_set_signed_ui(q, au / bu, (a->size ^ b->size));
    } else if (lmpz_size(b) == 1) {
        size_t sa = lmpz_size(a);
        if (LMP_UNLIKELY(err = lmpz_reserve(q, sa))) return err;
        lmpz_set_signed_ui(r, lmpn_div_qr_1(lmpz_ptr(q), lmpz_ptr(a), sa, lmpz_get_ui(b)), lmpz_sgn_bit(a));
        q->size = lmpz_gen_size(sa - !lmpz_at(q, sa-1), lmpz_sgn_bit(a));
    } else {
        size_t sa = lmpz_size(a), sb = lmpz_size(b);
        if (LMP_UNLIKELY((err = lmpz_reserve(q, sa-sb+1)) || (err = lmpz_reserve(r, sb)) || (err = lmpn_div_qr(lmpz_ptr(q), lmpz_ptr(r), lmpz_ptr(a), sa, lmpz_ptr(b), sb)))) return err;
        q->size = lmpz_gen_size(sa-sb+!!lmpz_at(q, sa-sb), (a->size ^ b->size) & LMPZ_SIGN_BIT);
        r->size = lmpz_gen_size(lmpn_norm(lmpz_ptr(r), sb), lmpz_sgn_bit(a));
    }
    return 0;
}