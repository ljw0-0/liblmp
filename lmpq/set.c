#include "lmp.h"

int lmpq_set(lmpq_t q, const lmpq_t a) {
    return lmpz_set(lmpq_numref(q), lmpq_numref(a)) || lmpz_set(lmpq_denref(q), lmpq_denref(a));
}