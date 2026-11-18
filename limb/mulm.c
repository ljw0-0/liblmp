#include "lmp.h"
#include "impl.h"

limb_t limb_mulm(limb_t a, limb_t b, limb_t m) {
    #ifdef tmp_t
    return (limb_t)(((tmp_t)a * (tmp_t)b) % m);
    #else
    if (m == 0) return 0;
    if (a == 0 || b == 0) return 0;
    if (a >= m) a %= m;
    if (b >= m) b %= m;

    limb_t result = 0;
    while (b > 0) {
        if (b & 1) {
            result += a;
            if (result >= m) result -= m;
        }
        a <<= 1;
        if (a >= m) a -= m;
        b >>= 1;
    }
    return result;
    #endif
}