#include "lmp.h"
#include <string.h>

int lmpn_sub_1(limb_t *c, const limb_t *a, size_t n, limb_t b) {
    limb_t d = *a;
    *c = d - b;
    if (d >= b) {
        copy:
        if (a != c && n > 1) memcpy(c+1, a+1, (n-1)*sizeof(limb_t));
        return 0;
    }
    while (--n) {
        d = *(++a);
        *(++c) = d - 1;
        if (d) goto copy;
    }
    return 1;
}
