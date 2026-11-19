#include "lmp.h"
#include <string.h>

int lmpn_bdiv_q(limb_t *q, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(b[0] & 1 && n >= m);
    limb_t *tmp = lmpn_alloc(n);
    if (LMP_UNLIKELY(!tmp)) return 1;
    memcpy(tmp, a, n*sizeof(limb_t));
    int r = _lmpn_bdiv_q(q, tmp, n, b, m);
    free(tmp);
    return r;
}