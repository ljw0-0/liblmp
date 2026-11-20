#include "lmp.h"

void lmpq_clear(lmpq_t q) {
    lmpz_clear(lmpq_numref(q));
    lmpz_clear(lmpq_denref(q));
}