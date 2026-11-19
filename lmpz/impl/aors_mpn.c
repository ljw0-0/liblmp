#include "lmp.h"
#include "impl.h"

int _lmpz_aors_mpn(lmpz_t z, const lmpz_t a, const limb_t *bp, size_t bn, size_t sign_bit) {
    if (lmpz_sgn_bit(a) ^ sign_bit) {
        if (lmpz_size(a) <= 1) return _lmpz_sub_mpn_ui(z, lmpz_sgn_bit(a) ^ sign_bit, bp, bn, lmpz_get_ui(a));
        if (lmpz_size(a) > bn) return _lmpz_sub_mpn_mpn(z, lmpz_sgn_bit(a), lmpz_limbs(a), lmpz_size(a), bp, bn);
        if (lmpz_size(a) < bn) return _lmpz_sub_mpn_mpn(z, lmpz_sgn_bit(a) ^ sign_bit, bp, bn, lmpz_limbs(a), lmpz_size(a));
        int r = lmpz_reserve(z, lmpz_size(a));
        if (LMP_UNLIKELY(r)) return r;
        if (mpn_sub_n(lmpz_ptr(z), lmpz_ptr(a), bp, bn)) mpn_neg(lmpz_ptr(z), lmpz_ptr(z), bn);
        z->size = mpn_norm(lmpz_ptr(z), bn) | lmpz_sgn_bit(a);
        return 0;
    }
    if (lmpz_size(a) <= 1) return _lmpz_add_mpn_ui(z, lmpz_sgn_bit(a), bp, bn, lmpz_get_ui(a));
    return (lmpz_size(a) >= bn) ? _lmpz_add_mpn_mpn(z, lmpz_sgn_bit(a), lmpz_limbs(a), lmpz_size(a), bp, bn) : _lmpz_add_mpn_mpn(z, lmpz_sgn_bit(a), bp, bn, lmpz_limbs(a), lmpz_size(a));
}