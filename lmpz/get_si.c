#include "lmp.h"

slimb_t lmpz_get_si(const lmpz_t z) {
    limb_t go = (lmpz_cap(z) ? lmpz_at(z, 0) : z->limb) & (LIMB_MAX >> 1);
    return (z->size & LMPZ_SIGN_BIT) ? -go : go;
}