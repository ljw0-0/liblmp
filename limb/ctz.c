#include "lmp.h"

#ifndef limb_ctz
int limb_ctz(limb_t x) {
    int ans = 0;
    while (!(x & 1)) {
        x >>= 1;
        ++ans;
    }
    return ans;
}
#endif