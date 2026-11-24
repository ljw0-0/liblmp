#include "lmp.h"

int lmpz_ui_sub(lmpz_t z, limb_t a, const lmpz_t b) {
    limb_t bl;
    if (lmpz_sign_bit(b)) return lmpz_add_ui(z, b, a);
    if (lmpz_size(b) > 1) return _lmpz_sub_mpn_ui(z, LMPZ_SIGN_BIT, lmpz_ptr(b), lmpz_size(b), a);
    bl = lmpz_get_ui(b);
    if (bl >= a) lmpz_set_signed_ui(z, bl-a, LMPZ_SIGN_BIT);
    else lmpz_set_signed_ui(z, a-bl, 0);
    return 0;
}