#include "lmp.h"

void lmpz_one(lmpz_t z) {
    z->size = 1;
    if (z->alloc) lmpz_at(z, 0) = 1;
    else z->limb = 1;
}