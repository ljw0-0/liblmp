#include "lmp.h"

limb_t limb_exgcd(limb_t a, limb_t b, slimb_t *x, slimb_t *y) {
    limb_t x1=1, x2=0, x3=0, x4=1;
    while (b) {
        limb_t c = a/b, tmp;
        #define op(u, v) tmp = u; u = v; v = tmp - c*v;
        op(a, b)
        op(x1, x3)
        op(x2, x4)
        #undef op
    }
    *x = (x1 > (LIMB_MAX >> 1) ? -(slimb_t)(-x1) : (slimb_t)x1);
    *y = (x2 > (LIMB_MAX >> 1) ? -(slimb_t)(-x2) : (slimb_t)x2);
    return a;
}