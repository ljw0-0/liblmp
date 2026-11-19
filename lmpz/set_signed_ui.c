#include "lmp.h"

void lmpz_set_signed_ui(lmpz_t z, limb_t a, size_t sign_bit) {
    if (lmpz_cap(z)) lmpz_at(z, 0) = a;
    else z->limb = a;
    z->size = lmpz_gen_size(!!a, sign_bit);
}