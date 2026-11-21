#include "lmp.h"

void lmpq_swap(lmpq_t x, lmpq_t y) {
    lmpz_swap(lmpq_numref(x), lmpq_numref(y));
    lmpz_swap(lmpq_denref(x), lmpq_denref(y));
}