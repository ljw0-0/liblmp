#include "lmp.h"

void lmpz_init(lmpz_t z) {
    z->alloc = 0;
    z->size = 0;
    z->limb = 0;
}