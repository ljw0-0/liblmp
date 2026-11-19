#include "lmp.h"

int lmpz_sizeinbase(const lmpz_t z, int base) {
    return mpn_sizeinbase(lmpz_get_mpn(z), lmpz_size(z), base);
}