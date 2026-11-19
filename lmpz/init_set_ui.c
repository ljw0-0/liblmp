#include "lmp.h"

void lmpz_init_set_ui(lmpz_t z, limb_t a) {
    _lmpz_init_set(z, !!a, 0, a);
}