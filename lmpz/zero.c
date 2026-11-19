#include "lmp.h"

void lmpz_zero(lmpz_t z) {
    z->size = 0;
    if (z->alloc) lmpz_at(z, 0) = 0;
    else z->limb = 0;
}