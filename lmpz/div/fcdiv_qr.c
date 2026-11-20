#include "lmp.h"

static int lmpz_fcdiv_qr(lmpz_t q, lmpz_t r, const lmpz_t a, const lmpz_t b, size_t sign_bit) {
    int err;
    if (LMP_UNLIKELY(lmpz_is_zero(b))) return LMP_EDIVZERO;
    size_t sa = lmpz_size(a), sb = lmpz_size(b);
    size_t sign_a = lmpz_sgn_bit(a), sign_b = lmpz_sgn_bit(b);
    int cdiv = ((sign_a ^ sign_b ^ sign_bit) & LMPZ_SIGN_BIT) != 0;

    if (lmpz_is_zero(a)) {
        lmpz_zero(q);
        lmpz_zero(r);
    } else if (sa < sb) {
        if (!cdiv) {
            if (LMP_UNLIKELY(err = lmpz_set(r, a))) return err;
            lmpz_zero(q);
        } else {
            if (LMP_UNLIKELY(err = lmpz_reserve(r, sb))) return err;
            #ifdef NDEBUG
            lmpn_sub(lmpz_ptr(r), lmpz_get_mpn(b), sb, lmpz_get_mpn(a), sa);
            #else
            int borrow = lmpn_sub(lmpz_ptr(r), lmpz_get_mpn(b), sb, lmpz_get_mpn(a), sa);
            assert(!borrow);
            #endif
            r->size = lmpz_gen_size(lmpn_norm(lmpz_ptr(r), sb), sign_b ^ sign_bit);
            lmpz_set_signed_ui(q, 1, sign_a ^ sign_b);
        }
    } else if (sa == 1) {
        limb_t au = lmpz_get_ui(a), bu = lmpz_get_ui(b);
        limb_t rem = au % bu, quot = au / bu;
        if (cdiv && rem) {
            ++quot;
            rem = bu - rem;
        }
        lmpz_set_signed_ui(q, quot, sign_a ^ sign_b);
        lmpz_set_signed_ui(r, rem, rem ? (cdiv ? sign_b ^ sign_bit : sign_a) : 0);
    } else if (sb == 1) {
        if (LMP_UNLIKELY(err = lmpz_reserve(q, sa))) return err;
        limb_t bu = lmpz_get_ui(b);
        limb_t rem = lmpn_div_qr_1(lmpz_ptr(q), lmpz_ptr(a), sa, bu);
        if (cdiv && rem) {
            #ifdef NDEBUG
            lmpn_add_1(lmpz_ptr(q), lmpz_ptr(q), sa, 1);
            #else
            limb_t carry = lmpn_add_1(lmpz_ptr(q), lmpz_ptr(q), sa, 1);
            assert(!carry);
            #endif
            rem = bu - rem;
        }
        q->size = lmpz_gen_size(sa - !lmpz_at(q, sa-1), sign_a ^ sign_b);
        lmpz_set_signed_ui(r, rem, rem ? (cdiv ? sign_b ^ sign_bit : sign_a) : 0);
    } else {
        size_t qn = sa - sb + 1;
        if (LMP_UNLIKELY((err = lmpz_reserve(q, qn)) || (err = lmpz_reserve(r, sb)))) return err;
        if (cdiv) {
            if (LMP_UNLIKELY(err = lmpn_cdiv_qr(lmpz_ptr(q), lmpz_ptr(r), lmpz_ptr(a), sa, lmpz_ptr(b), sb))) return err;
            r->size = lmpz_gen_size(lmpn_norm(lmpz_ptr(r), sb), sign_b ^ sign_bit);
        } else {
            if (LMP_UNLIKELY(err = lmpn_div_qr(lmpz_ptr(q), lmpz_ptr(r), lmpz_ptr(a), sa, lmpz_ptr(b), sb))) return err;
            r->size = lmpz_gen_size(lmpn_norm(lmpz_ptr(r), sb), sign_a);
        }
        q->size = lmpz_gen_size(qn - !lmpz_at(q, qn-1), sign_a ^ sign_b);
    }
    return 0;
}

int lmpz_fdiv_qr(lmpz_t q, lmpz_t r, const lmpz_t a, const lmpz_t b) {return lmpz_fcdiv_qr(q, r, a, b, 0);}
int lmpz_cdiv_qr(lmpz_t q, lmpz_t r, const lmpz_t a, const lmpz_t b) {return lmpz_fcdiv_qr(q, r, a, b, LMPZ_SIGN_BIT);}