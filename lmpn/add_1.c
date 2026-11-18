#include "lmp.h"
#include <string.h>

int lmpn_add_1(limb_t *c, const limb_t *a, size_t n, limb_t b) {
    if ((*c = *a + b) >= b) {
        copy:
        if (a != c && n > 1) memcpy(c+1, a+1, (n-1)*sizeof(limb_t));
        return 0;
    }
    while (--n)  if ((*(++c) = *(++a) + 1)) goto copy;
    return 1;
}