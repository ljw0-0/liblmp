#include "lmp.h"

#ifndef limb_clz
int limb_clz(limb_t x) {
    int ans = 0;
    while (!(x >> (LIMB_WIDTH - 1))) {
        x <<= 1;
        ++ans;
    }
    return ans;
}
#endif