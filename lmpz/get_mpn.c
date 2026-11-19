#include "lmp.h"

const limb_t *lmpz_get_mpn(const lmpz_t z) {
    return lmpz_cap(z) ? lmpz_limbs(z) : &z->limb;
}