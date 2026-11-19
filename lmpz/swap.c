#include "lmp.h"

void lmpz_swap(lmpz_t a, lmpz_t b) {
    lmpz tmp = *a;
    *a = *b;
    *b = tmp;
}
