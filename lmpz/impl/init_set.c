#include "lmp.h"

void _lmpz_init_set(lmpz_t z, size_t sz, size_t alloc, uintptr_t limb) {
    z->size = sz;
    z->alloc = alloc;
    z->limb = limb;
}
