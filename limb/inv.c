#include "lmp.h"

limb_t limb_inv(limb_t a) {
    assert(a & 1);

    limb_t x = 2-a;
    x *= 2-a*x;
    #if LIMB_WIDTH > 2
    x *= 2-a*x;
    #endif
    #if LIMB_WIDTH > 4
    x *= 2-a*x;
    #endif
    #if LIMB_WIDTH > 8
    x *= 2-a*x;
    #endif
    #if LIMB_WIDTH > 16
    x *= 2-a*x;
    #endif
    #if LIMB_WIDTH > 32
    x *= 2-a*x;
    #endif
    #if LIMB_WIDTH > 64
    x *= 2-a*x;
    #endif
    #if LIMB_WIDTH > 128
    x *= 2-a*x;
    #endif
    #if LIMB_WIDTH > 256
    # error "Unsupported limb."
    #endif
    return x;
}