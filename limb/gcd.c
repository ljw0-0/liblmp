#include "lmp.h"

limb_t limb_gcd(limb_t a, limb_t b) {
    if (!a || !b) return a | b;
    int sa = limb_ctz(a), sb = limb_ctz(b), s = sa<sb ? sa : sb;
    a >>= sa;
    b >>= sb;
    while (a ^ b) {
        if (a > b) {
            a -= b;
            a >>= limb_ctz(a);
        } else {
            b -= a;
            b >>= limb_ctz(b);
        }
    }
    return a << s;
}