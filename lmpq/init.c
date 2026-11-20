#include "lmp.h"

void lmpq_init(lmpq_t q) {
    lmpz_init(lmpq_numref(q));
    _lmpz_init_set(lmpq_denref(q), 1, 0, 1);
}