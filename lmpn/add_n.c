#include "lmp.h"

int lmpn_add_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n) {
    int carry = 0;
    do {
        limb_t d = *a, sum = d + *b;
        *c = sum + carry;
        carry = (sum < d) | (*c < sum);
        ++a; ++b; ++c;
    } while (--n);
    return carry;
}
