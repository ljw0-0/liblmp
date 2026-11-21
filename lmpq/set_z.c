#include "lmp.h"

int lmpq_set_z(lmpq_t q, const lmpz_t a) {
    int err = lmpq_set_num(q, a);
    if (LMP_UNLIKELY(err)) return err;
    lmpz_one(lmpq_denref(q));
    return 0;
}