#include "lmp.h"

int lmpz_tdiv_q(lmpz_t q, const lmpz_t a, const lmpz_t b) {
    int err;
    if (LMP_UNLIKELY(lmpz_is_zero(b))) return LMP_EDIVZERO;
    if (lmpz_size(a) < lmpz_size(b)) lmpz_zero(q);
    else if (lmpz_size(a) == 1) lmpz_set_signed_ui(q, lmpz_get_ui(a) / lmpz_get_ui(b), (a->size ^ b->size));
    else if (lmpz_size(b) == 1) {
        size_t sa = lmpz_size(a);
        if (LMP_UNLIKELY(err = lmpz_reserve(q, sa))) return err;
        lmpn_div_qr_1(lmpz_ptr(q), lmpz_ptr(a), sa, lmpz_get_ui(b));
        q->size = lmpz_gen_size(sa - !lmpz_at(q, sa-1), lmpz_sign_bit(a));
    } else {
        size_t sa = lmpz_size(a), sb = lmpz_size(b);
        if (LMP_UNLIKELY((err = lmpz_reserve(q, sa-sb+1))|| (err = lmpn_div_q(lmpz_ptr(q), lmpz_ptr(a), sa, lmpz_ptr(b), sb)))) return err;
        q->size = lmpz_gen_size(sa-sb+!!lmpz_at(q, sa-sb), (a->size ^ b->size) & LMPZ_SIGN_BIT);
    }
    return 0;
}