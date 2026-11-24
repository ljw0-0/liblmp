#include "lmp.h"

int _lmpz_mul_ui(lmpz_t z, const lmpz_t a, limb_t b, size_t sign_bit) {
    if (lmpz_is_zero(a) || !b) {
        lmpz_zero(z);
        return 0;
    }
    if (lmpz_size(a) <= 1) {
        limb_t hi, lo;
        limb_mul(&hi, &lo, lmpz_get_ui(a), b);
        return lmpz_set_signed_uiui(z, hi, lo, lmpz_sign_bit(a) ^ sign_bit);
    }
    return _lmpz_mul_mpn_ui(z, lmpz_sign_bit(a) ^ sign_bit, lmpz_ptr(a), lmpz_size(a), b);
}