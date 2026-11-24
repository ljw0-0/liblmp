#include "lmp.h"

static int lmpz_fcdiv_r(lmpz_t r, const lmpz_t a, const lmpz_t b, size_t sign_bit) {
    int err;
    if (LMP_UNLIKELY(lmpz_is_zero(b))) return LMP_EDIVZERO;
    size_t sa = lmpz_size(a), sb = lmpz_size(b);
    size_t sign_a = lmpz_sign_bit(a), sign_b = lmpz_sign_bit(b);
    int cdiv = ((sign_a ^ sign_b ^ sign_bit) & LMPZ_SIGN_BIT) != 0;

    if (lmpz_is_zero(a)) {
        lmpz_zero(r);
    } else if (sa < sb) {
        if (!cdiv) {
            if (LMP_UNLIKELY(err = lmpz_set(r, a))) return err;
        } else {
            if (LMP_UNLIKELY(err = lmpz_reserve(r, sb))) return err;
            #ifdef NDEBUG
            lmpn_sub(lmpz_ptr(r), lmpz_get_mpn(b), sb, lmpz_get_mpn(a), sa);
            #else
            int borrow = lmpn_sub(lmpz_ptr(r), lmpz_get_mpn(b), sb, lmpz_get_mpn(a), sa);
            assert(!borrow);
            #endif
            r->size = lmpz_gen_size(lmpn_norm(lmpz_ptr(r), sb), sign_b ^ sign_bit);
        }
    } else if (sa == 1) {
        limb_t au = lmpz_get_ui(a), bu = lmpz_get_ui(b);
        limb_t rem = au % bu;
        if (cdiv && rem) rem = bu - rem;
        lmpz_set_signed_ui(r, rem, rem ? (cdiv ? sign_b ^ sign_bit : sign_a) : 0);
    } else if (sb == 1) {
        limb_t bu = lmpz_get_ui(b);
        limb_t rem = lmpn_div_r_1(lmpz_ptr(a), sa, bu);
        if (cdiv && rem) rem = bu - rem;
        lmpz_set_signed_ui(r, rem, rem ? (cdiv ? sign_b ^ sign_bit : sign_a) : 0);
    } else {
        if (LMP_UNLIKELY((err = lmpz_reserve(r, sb)))) return err;
        if (cdiv) {
            if (LMP_UNLIKELY(err = lmpn_cdiv_r(lmpz_ptr(r), lmpz_ptr(a), sa, lmpz_ptr(b), sb))) return err;
            r->size = lmpz_gen_size(lmpn_norm(lmpz_ptr(r), sb), sign_b ^ sign_bit);
        } else {
            if (LMP_UNLIKELY(err = lmpn_div_r(lmpz_ptr(r), lmpz_ptr(a), sa, lmpz_ptr(b), sb))) return err;
            r->size = lmpz_gen_size(lmpn_norm(lmpz_ptr(r), sb), sign_a);
        }
    }
    return 0;
}

int lmpz_fdiv_r(lmpz_t r, const lmpz_t a, const lmpz_t b) {return lmpz_fcdiv_r(r, a, b, 0);}
int lmpz_cdiv_r(lmpz_t r, const lmpz_t a, const lmpz_t b) {return lmpz_fcdiv_r(r, a, b, LMPZ_SIGN_BIT);}