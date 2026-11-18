#include "lmp.h"

limb_t limb_invm(limb_t a, limb_t m) {
    slimb_t x, y;
    #ifdef NDEBUG
    limb_exgcd(a, m, &x, &y);
    #else
    assert(limb_exgcd(a, m, &x, &y) == 1);
    #endif
    return x >= 0 ? x : m+x;
}