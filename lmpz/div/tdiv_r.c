#include "lmp.h"

int lmpz_tdiv_r(lmpz_t r, const lmpz_t a, const lmpz_t b) {
    int err;
    if (LMP_UNLIKELY(lmpz_is_zero(b))) return LMP_EDIVZERO;
    if (lmpz_is_zero(a)) lmpz_zero(r);
    else if (lmpz_size(a) < lmpz_size(b)) return lmpz_set(r, a);
    else if (lmpz_size(a) == 1) lmpz_set_signed_ui(r, lmpz_get_ui(a) % lmpz_get_ui(b), a->size ^ 1);
    else if (lmpz_size(b) == 1) lmpz_set_signed_ui(r, lmpn_div_r_1(lmpz_ptr(a), lmpz_size(a), lmpz_get_ui(b)), lmpz_sgn_bit(a));
    else {
        size_t sb = lmpz_size(b);
        if (LMP_UNLIKELY((err = lmpz_reserve(r, sb)) || (err = lmpn_div_r(lmpz_ptr(r), lmpz_ptr(a), lmpz_size(a), lmpz_ptr(b), sb)))) return err;
        r->size = lmpz_gen_size(lmpn_norm(lmpz_ptr(r), sb), lmpz_sgn_bit(a));
    }
    return 0;
}