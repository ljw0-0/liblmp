#include "lmp.h"

void lmpz_init_set_signed_ui(lmpz_t z, limb_t a, size_t sign_bit) {
    _lmpz_init_set(z, lmpz_gen_size(!!a, sign_bit), 0, a);
}