#include "lmp.h"
#include "impl.h"

limb_t limb_mul_hi(limb_t a, limb_t b) {
    #ifdef tmp_t
    return ((tmp_t)a * (tmp_t)b) >> LIMB_WIDTH;
    #else
    # error "Unsupported limb."
    #endif
}