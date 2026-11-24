#include "lmp.h"

int lmpq_set_str(lmpq_t q, const char *str, int base) {
    return _lmpq_set_str(lmpq_numref(q), lmpq_denref(q), str, base);
}