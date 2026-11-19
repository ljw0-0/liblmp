#include "lmp.h"

void lmpz_set_ui(lmpz_t z, limb_t a) {
    if (lmpz_cap(z)) lmpz_at(z, 0) = a;
    else z->limb = a;
    z->size = !!a;
}