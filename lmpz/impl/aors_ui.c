#include "lmp.h"

int _lmpz_aors_ui(lmpz_t z, const lmpz_t a, limb_t b, size_t sign_bit) {
    limb_t al;
    if (lmpz_sgn_bit(a) ^ sign_bit) {
        if (lmpz_cap(a)) return _lmpz_add_mpn_ui(z, sign_bit, lmpz_ptr(a), a->size ^ sign_bit, b);
        return lmpz_set_signed_uiui(z, lmpz_limb(a) + b < b, b, sign_bit);
    }
    if (lmpz_size(a) > 1) return _lmpz_sub_mpn_ui(z, sign_bit^LMPZ_SIGN_BIT, lmpz_ptr(a), lmpz_size(a), b);
    al = lmpz_get_ui(a);
    if (al >= b) lmpz_set_signed_ui(z, al-b, sign_bit^LMPZ_SIGN_BIT);
    else lmpz_set_signed_ui(z, b-al, sign_bit);
    return 0;
}