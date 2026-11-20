#include "lmp.h"
#include <string.h>

int lmpz_gcd(lmpz_t g, const lmpz_t a, const lmpz_t b) {
    if (lmpz_is_zero(b)) return lmpz_abs(g, a);
    if (lmpz_is_zero(a)) return lmpz_abs(g, b);
    if (lmpz_size(a) == 1) lmpz_set_ui(g, lmpz_gcd_ui(b, lmpz_get_ui(a)));
    else if (lmpz_size(b) == 1) lmpz_set_ui(g, lmpz_gcd_ui(a, lmpz_get_ui(b)));
    else {
        size_t an = lmpz_size(a), bn = lmpz_size(b), gn;
        limb_t *ap, *bp;
        if (LMP_UNLIKELY(!(ap = lmpn_alloc(an)))) return LMP_ENOMEM;
        if (LMP_UNLIKELY(!(bp = lmpn_alloc(bn)))) goto cleanup;
        memcpy(ap, lmpz_ptr(a), an * sizeof(limb_t));
        memcpy(bp, lmpz_ptr(b), bn * sizeof(limb_t));
        if (LMP_UNLIKELY(an >= bn ? (lmpz_reserve(g, an) || !(gn = lmpn_gcd(lmpz_ptr(g), ap, an, bp, bn))) : (lmpz_reserve(g, bn) || !(gn = lmpn_gcd(lmpz_ptr(g), bp, bn, ap, an))))) {
            free(bp);
            cleanup:
            free(ap);
            return LMP_ENOMEM;
        }
        g->size = gn;
        free(ap);
        free(bp);
    }
    return 0;
}