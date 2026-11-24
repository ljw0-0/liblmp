#include "lmp.h"

int lmpz_divexact(lmpz_t q, const lmpz_t a, const lmpz_t b) {
    int err;
    if (LMP_UNLIKELY(lmpz_is_zero(b))) return LMP_EDIVZERO;
    if (lmpz_is_zero(a)) {
        lmpz_zero(q);
        return 0;
    }

    size_t sa = lmpz_size(a), sb = lmpz_size(b);
    assert(sa >= sb);

    if (sa == 1) {
        lmpz_set_signed_ui(q, lmpz_get_ui(a) / lmpz_get_ui(b), lmpz_sign_bit(a) ^ lmpz_sign_bit(b));
        return 0;
    }

    size_t qn = sa - sb + 1;
    if (LMP_UNLIKELY(err = lmpz_reserve(q, qn))) return err;
    if (LMP_UNLIKELY(err = lmpn_divexact(lmpz_ptr(q), lmpz_get_mpn(a), sa, lmpz_get_mpn(b), sb))) return err;
    q->size = lmpz_gen_size(lmpn_norm(lmpz_ptr(q), qn), lmpz_sign_bit(a) ^ lmpz_sign_bit(b));
    return 0;
}