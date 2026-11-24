#include "lmp.h"

char *lmpq_get_str(char *str, int base, const lmpq_t q) {
    return _lmpq_get_str(str, base, lmpq_numref(q), lmpq_denref(q));
}