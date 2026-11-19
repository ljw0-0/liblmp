#include "lmp.h"

int lmpz_mul(lmpz_t z, const lmpz_t a, const lmpz_t b) {
    return lmpz_size(b) > 1 ? _lmpz_mul_mpn(z, a, lmpz_ptr(b), lmpz_size(b), lmpz_sgn_bit(b)) : _lmpz_mul_ui(z, a, lmpz_get_ui(b), lmpz_sgn_bit(b));
}