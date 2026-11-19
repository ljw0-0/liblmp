#include "lmp.h"

int lmpz_sign(lmpz_t z) {
    #ifdef SIZE_WIDTH
    return (int)(z->size != 0) - ((int)((z->size) >> (SIZE_WIDTH - 1)) << 1);
    #elif defined(__SIZE_WIDTH__)
    return (int)(z->size != 0) - ((int)((z->size) >> (__SIZE_WIDTH__ - 1)) << 1);
    #else
    return (int)(z->size != 0) - ((z->size > (SIZE_MAX>>1)) << 1);
    #endif
}
