#include "lmp.h"

int lmpz_is_pm1(const lmpz_t z) {
    return lmpz_size(z) == 1 && lmpz_get_ui(z) == 1;
}