#include "lmp.h"

void lmpz_init_set_si(lmpz_t z, slimb_t a) {
    if (a >= 0) lmpz_init_set_ui(z, a);
    else lmpz_init_neg_ui(z, -(limb_t)a);
}