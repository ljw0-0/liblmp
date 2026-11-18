#include "lmp.h"
#include "impl.h"

#ifdef tmp_t
limb_t limb_div(limb_t hi, limb_t lo, limb_t d) {
    return ((tmp_t)hi << LIMB_WIDTH | (tmp_t)lo) / d;
}
#elif defined(_MSC_VER) && _MSC_VER >= 1920 && defined(_M_X64) && LIMB_WIDTH == 64
#include <immintrin.h>
limb_t limb_div(limb_t hi, limb_t lo, limb_t d) {
    limb_t ui;
    return _udiv128(hi, lo, d, &ui);
}
#else
# error "Unsupported limb."
#endif