#include "lmp.h"

int lmpz_equal_ui(const lmpz_t a, limb_t b) {
    return a->size <= 1 && lmpz_get_ui(a) == b;
}