#include "lmp.h"

void lmpq_set_si(lmpq_t q, slimb_t num, limb_t den) {
    lmpz_set_si(lmpq_numref(q), num);
    lmpz_set_ui(lmpq_denref(q), num ? den : 1);
}