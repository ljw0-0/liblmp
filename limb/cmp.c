#include "lmp.h"

int limb_cmp(limb_t a, limb_t b) {
    return (a>b) - (a<b);
}
