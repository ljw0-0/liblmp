#include "lmp.h"

int _lmpz_aors(lmpz_t z, const lmpz_t a, const lmpz_t b, size_t sign_bit) {
    if (lmpz_cap(b)) return _lmpz_aors_mpn(z, a, lmpz_limbs(b), lmpz_size(b), lmpz_sgn_bit(b) ^ sign_bit);
    return _lmpz_aors_ui(z, a, lmpz_limb(b), lmpz_sgn_bit(b) ^ sign_bit);
}