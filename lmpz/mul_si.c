#include "lmp.h"

int lmpz_mul_si(lmpz_t z, const lmpz_t a, slimb_t b) {
    return _lmpz_mul_ui(z, a, b < 0 ? -(limb_t)b : (limb_t)b, lmpz_sign_bit(a) ^ (b < 0 ? LMPZ_SIGN_BIT : 0));
}