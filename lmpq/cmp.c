#include "lmp.h"

int lmpq_cmp(const lmpq_t a, const lmpq_t b) {
    return _lmpq_cmp(
        lmpq_numref(a), lmpq_denref(a),
        lmpq_numref(b), lmpq_denref(b));
}

int lmpq_cmp_z(const lmpq_t a, const lmpz_t b) {
    return _lmpq_cmp_z(lmpq_numref(a), lmpq_denref(a), b);
}

int lmpq_cmp_si(const lmpq_t a, slimb_t b) {
    return _lmpq_cmp_si(lmpq_numref(a), lmpq_denref(a), b);
}

int lmpq_cmp_ui(const lmpq_t a, limb_t b) {
    return _lmpq_cmp_ui(lmpq_numref(a), lmpq_denref(a), b);
}