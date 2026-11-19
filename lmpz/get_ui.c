#include "lmp.h"

limb_t lmpz_get_ui(const lmpz_t z) {
    return lmpz_cap(z) ? lmpz_at(z, 0) : z->limb;
}