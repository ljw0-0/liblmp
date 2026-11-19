#include "lmp.h"
#include <stdlib.h>

void lmpz_clear(lmpz_t z) {
    if (lmpz_cap(z)) free(lmpz_ptr(z));
}