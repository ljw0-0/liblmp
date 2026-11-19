#include "lmp.h"

int lmpz_is_one(const lmpz_t z) {
    return z->size == 1 && lmpz_get_ui(z) == 1;
}