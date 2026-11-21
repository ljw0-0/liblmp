#include "lmp.h"

int lmpq_canonicalize(lmpq_t q) {
    int err;
    lmpz_t g;
    if (LMP_UNLIKELY(!(q->den.size))) return LMP_EDIVZERO;
    lmpz_init(g);
    err = lmpz_gcd(g, lmpq_numref(q), lmpq_denref(q)) || lmpz_divexact(lmpq_numref(q), lmpq_numref(q), g) || lmpz_divexact(lmpq_denref(q), lmpq_denref(q), g);
    lmpz_clear(g);
    return err;
}