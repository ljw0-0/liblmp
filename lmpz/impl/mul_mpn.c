#include "lmp.h"

int _lmpz_mul_mpn(lmpz_t z, const lmpz_t a, limb_t *bp, size_t bn, size_t sign_bit) {
    if (lmpz_is_zero(a) || !bn) {
        lmpz_zero(z);
        return 0;
    }
    if (lmpz_size(a) <= 1) return _lmpz_mul_mpn_ui(z, lmpz_sign_bit(a) ^ sign_bit, bp, bn, lmpz_get_ui(a));
    return _lmpz_mul_mpn_mpn(z, lmpz_sign_bit(a) ^ sign_bit, lmpz_ptr(a), lmpz_size(a), bp, bn);
}