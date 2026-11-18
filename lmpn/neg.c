#include "lmp.h"

void lmpn_neg(limb_t *b, const limb_t *a, size_t n) {
    int carry = 1;
    do {
        limb_t t = ~(*a) + carry;
        carry = t < carry;
        *b = t;
        ++a;
        ++b;
    } while (--n);
}