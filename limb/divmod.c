#include "lmp.h"
#include "impl.h"

#ifdef tmp_t
limb_t limb_divmod(limb_t *q, limb_t hi, limb_t lo, limb_t d) {
    tmp_t res = ((tmp_t)hi << LIMB_WIDTH | (tmp_t)lo);
    *q = res / d;
    return res % d;
}
#elif defined(_MSC_VER) && _MSC_VER >= 1920 && defined(_M_X64) && LIMB_WIDTH == 64
#include <immintrin.h>
limb_t limb_divmod(limb_t *q, limb_t hi, limb_t lo, limb_t d) {
    limb_t r;
    *q = _udiv128(hi, lo, d, &r);
    return r;
}
#else
# error "Unsupported limb."
#endif