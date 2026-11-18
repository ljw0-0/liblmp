#include "lmp.h"

limb_t limb_powm(limb_t a, limb_t e, limb_t m) {
    limb_t res = 1;
    while (e) {
        if (e & 1) res = limb_mulm(res, a, m);
        a = limb_mulm(a, a, m);
        e >>= 1;
    }
    return res;
}

limb_t limb_invm_p(limb_t a, limb_t p) {return limb_powm(a, p-2, p);}