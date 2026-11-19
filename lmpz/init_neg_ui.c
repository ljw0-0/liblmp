#include "lmp.h"

void lmpz_init_neg_ui(lmpz_t z, limb_t a) {
    _lmpz_init_set(z, lmpz_neg_size(!!a), 0, a);
}