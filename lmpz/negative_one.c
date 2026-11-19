#include "lmp.h"

void lmpz_negative_one(lmpz_t z) {
    z->size = LMPZ_SIGN_BIT | 1;
    if (z->alloc) lmpz_at(z, 0) = 1;
    else z->limb = 1;
}