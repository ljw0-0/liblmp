#ifndef LMP_H
#define LMP_H
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------ MACROS ------ */

#if defined(__GNUC__) || defined(__clang__)
# define LMP_INLINE      static inline __attribute__((always_inline))
# define LMP_FLATTEN     __attribute__((flatten))
# define LMP_LIKELY(x)   __builtin_expect(!!(x), 1)
# define LMP_UNLIKELY(x) __builtin_expect(!!(x), 0)
# define LMP_PURE        __attribute__((pure))
# define LMP_CONST       __attribute__((const))
# define LMP_NORETURN    __attribute__((noreturn))
#elif defined(_MSC_VER)
# define LMP_INLINE      static __forceinline
# define LMP_FLATTEN
# define LMP_LIKELY(x)   (x)
# define LMP_UNLIKELY(x) (x)
# define LMP_PURE
# define LMP_CONST
# define LMP_NORETURN
#else
# define LMP_INLINE      static inline
# define LMP_FLATTEN
# define LMP_LIKELY(x)   (x)
# define LMP_UNLIKELY(x) (x)
# define LMP_PURE
# define LMP_CONST
# define LMP_NORETURN
#endif

/* ------ ERRNO ------ */

#define LMP_ENOMEM     1
#define LMP_EOVERFLOW  2
#define LMP_EDIVZERO   3
#define LMP_EBASE      4
#define LMP_EPARSE     5

/* ------ LIMB ------ */

#if (UINTPTR_MAX >> 1) != INTPTR_MAX
# error "Unsupported limb."
#endif

typedef uintptr_t limb_t;
typedef intptr_t slimb_t;
#define LIMB_MAX   UINTPTR_MAX

#ifdef UINTPTR_WIDTH
# define LIMB_WIDTH UINTPTR_WIDTH
#elif LIMB_MAX == 0xFFFFFFFFFFFFFFFF
# define LIMB_WIDTH 64
#elif LIMB_MAX == 0xFFFFFFFF
# define LIMB_WIDTH 32
#elif LIMB_MAX == 0xFFFF
# define LIMB_WIDTH 16
#else
# error "Unsupported limb."
#endif

// constants

#if LIMB_WIDTH == 16
#  define limb_inv3 ((limb_t)0xAAABu)
#elif LIMB_WIDTH == 32
#  define limb_inv3 ((limb_t)0xAAAAAAABu)
#elif LIMB_WIDTH == 64
#  define limb_inv3 ((limb_t)0xAAAAAAAAAAAAAAABull)
#else
#  error "unsupported LIMB_BITS"
#endif

// clz and ctz

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
# define limb_clz stdc_leading_zeros
# define limb_ctz stdc_trailing_zeros
#elif defined(__GNUC__)
# if LIMB_MAX == ULLONG_MAX
#  define limb_clz __builtin_clzll
#  define limb_ctz __builtin_ctzll
# elif LIMB_MAX == ULONG_MAX
#  define limb_clz __builtin_clzl
#  define limb_ctz __builtin_ctzl
# else
#  define limb_clz __builtin_clz
#  define limb_ctz __builtin_ctz
# endif
#else
int limb_clz(limb_t x);
int limb_ctz(limb_t x);
#endif

// compare

int limb_cmp(limb_t a, limb_t b);

// operation

void limb_mul(limb_t *restrict hi, limb_t *restrict lo, limb_t a, limb_t b);

limb_t limb_mul_hi(limb_t a, limb_t b);

limb_t limb_divmod(limb_t *q, limb_t hi, limb_t lo, limb_t d);
limb_t limb_div(limb_t hi, limb_t lo, limb_t d);
limb_t limb_mod(limb_t hi, limb_t lo, limb_t d);

// mod

limb_t limb_inv(limb_t a);

limb_t limb_mulm(limb_t a, limb_t b, limb_t m);
limb_t limb_powm(limb_t a, limb_t e, limb_t m);
limb_t limb_invm(limb_t a, limb_t m);
limb_t limb_invm_p(limb_t a, limb_t p);

limb_t limb_mul_mod(limb_t *restrict hi, limb_t *restrict lo, limb_t a, limb_t b, limb_t m);

// gcd

limb_t limb_gcd(limb_t a, limb_t b);
limb_t limb_exgcd(limb_t a, limb_t b, slimb_t *x, slimb_t *y);

/* ------ LMPN ------ */

#define LMPN_NTT_START 10

typedef struct {limb_t p, *wk, *wki;} lmpn_ntt_prime_t;

extern const lmpn_ntt_prime_t lmpn_ntt_primes[][3];
extern const size_t lmpn_ntt_prime_count;

// memory

#define LMPN_MAX_LEN (SIZE_MAX / sizeof(limb_t))

LMP_INLINE limb_t *lmpn_alloc(size_t sz) {
    return malloc(sz*sizeof(limb_t));
}

LMP_INLINE limb_t *lmpn_realloc(limb_t *p, size_t sz) {
    return realloc(p, sz*sizeof(limb_t));
}

// normalize

size_t lmpn_norm(const limb_t *a, size_t n);

// bit

size_t lmpn_clz(const limb_t *a, size_t n);
size_t lmpn_ctz(const limb_t *a, size_t n);

limb_t lmpn_lshift(limb_t *b, const limb_t *a, size_t n, int cnt);
limb_t lmpn_rshift(limb_t *b, const limb_t *a, size_t n, int cnt);

// compare

int lmpn_cmp(const limb_t *a, const limb_t *b, size_t n);
int lmpn_is_zero(const limb_t *a, size_t n);

// operation

void lmpn_neg(limb_t *b, const limb_t *a, size_t n);

int lmpn_add_1(limb_t *c, const limb_t *a, size_t n, limb_t b);
int lmpn_add_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);

LMP_INLINE int lmpn_add(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    return (n ^ m) ? lmpn_add_1(c+m, a+m, n-m, lmpn_add_n(c, a, b, m)) : lmpn_add_n(c, a, b, m);
}

int lmpn_sub_1(limb_t *c, const limb_t *a, size_t n, limb_t b);
int lmpn_sub_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);

LMP_INLINE int lmpn_sub(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    return (n ^ m) ? lmpn_sub_1(c+m, a+m, n-m, lmpn_sub_n(c, a, b, m)) : lmpn_sub_n(c, a, b, m);
}

limb_t lmpn_addlsh1_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);
limb_t lmpn_addlsh2_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);
limb_t lmpn_addlsh_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n, int cnt);

LMP_INLINE limb_t lmpn_addlsh1(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    return (n ^ m) ? (limb_t)lmpn_add_1(c+m, a+m, n-m, lmpn_addlsh1_n(c, a, b, m)) : lmpn_addlsh1_n(c, a, b, m);
}

LMP_INLINE limb_t lmpn_addlsh2(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    return (n ^ m) ? (limb_t)lmpn_add_1(c+m, a+m, n-m, lmpn_addlsh2_n(c, a, b, m)) : lmpn_addlsh2_n(c, a, b, m);
}

LMP_INLINE limb_t lmpn_addlsh(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m, int cnt) {
    assert(n >= m);
    return (n ^ m) ? (limb_t)lmpn_add_1(c+m, a+m, n-m, lmpn_addlsh_n(c, a, b, m, cnt)) : lmpn_addlsh_n(c, a, b, m, cnt);
}

limb_t lmpn_sublsh1_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);
limb_t lmpn_sublsh2_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);
limb_t lmpn_sublsh_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n, int cnt);

LMP_INLINE limb_t lmpn_sublsh1(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    return (n ^ m) ? (limb_t)lmpn_sub_1(c+m, a+m, n-m, lmpn_sublsh1_n(c, a, b, m)) : lmpn_sublsh1_n(c, a, b, m);
}

LMP_INLINE limb_t lmpn_sublsh2(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    return (n ^ m) ? (limb_t)lmpn_sub_1(c+m, a+m, n-m, lmpn_sublsh2_n(c, a, b, m)) : lmpn_sublsh2_n(c, a, b, m);
}

LMP_INLINE limb_t lmpn_sublsh(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m, int cnt) {
    assert(n >= m);
    return (n ^ m) ? (limb_t)lmpn_sub_1(c+m, a+m, n-m, lmpn_sublsh_n(c, a, b, m, cnt)) : lmpn_sublsh_n(c, a, b, m, cnt);
}

int lmpn_subrsh1_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);
int lmpn_subrsh2_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);
int lmpn_subrsh_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n, int cnt);


limb_t lmpn_mul_1(limb_t *c, const limb_t *a, size_t n, limb_t b);
limb_t lmpn_addmul_1(limb_t *c, const limb_t *a, size_t n, limb_t b);
limb_t lmpn_submul_1(limb_t *c, const limb_t *a, size_t n, limb_t b);

int lmpn_mul_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);
int lmpn_mul(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m);

#define lmpn_sqr(c, a, n) lmpn_mul_n(c, a, a, n)

// NTT
void lmpn_ntt_dit(limb_t *restrict a, size_t n, limb_t p, const limb_t *restrict wki);
void lmpn_ntt_dif(limb_t *restrict a, size_t n, limb_t p, const limb_t *restrict wk);

// division

limb_t lmpn_div_r_1(const limb_t *a, size_t n, limb_t b);
limb_t lmpn_div_qr_1(limb_t *q, const limb_t *a, size_t n, limb_t b);

#define lmpn_tdiv_r_1 lmpn_div_r_1
#define lmpn_tdiv_qr_1 lmpn_div_qr_1

int _lmpn_div_r(limb_t *a, size_t n, const limb_t *b, size_t m);
int _lmpn_div_qr(limb_t *q, limb_t *a, size_t n, const limb_t *b, size_t m);

int lmpn_div_q(limb_t *q, const limb_t *a, size_t n, const limb_t *b, size_t m);
int lmpn_div_r(limb_t *r, const limb_t *a, size_t n, const limb_t *b, size_t m);
int lmpn_div_qr(limb_t *q, limb_t *r, const limb_t *a, size_t n, const limb_t *b, size_t m);

int lmpn_cdiv_q(limb_t *q, const limb_t *a, size_t n, const limb_t *b, size_t m);
int lmpn_cdiv_r(limb_t *r, const limb_t *a, size_t n, const limb_t *b, size_t m);
int lmpn_cdiv_qr(limb_t *q, limb_t *r, const limb_t *a, size_t n, const limb_t *b, size_t m);

#define lmpn_tdiv_q lmpn_div_q
#define lmpn_tdiv_r lmpn_div_r
#define lmpn_tdiv_qr lmpn_div_qr

int _lmpn_bdiv_q(limb_t *q, limb_t *a, size_t n, const limb_t *b, size_t m);

int lmpn_bdiv_q(limb_t *q, const limb_t *a, size_t n, const limb_t *b, size_t m);

void lmpn_divexact_1(limb_t *q, const limb_t *a, size_t n, limb_t b);
void lmpn_divexact_by3(limb_t *q, const limb_t *a, size_t n);
int lmpn_divexact(limb_t *q, const limb_t *a, size_t n, const limb_t *b, size_t m);

//#define lmpn_divexact_by3(q, a, n) lmpn_divexact_1(q, a, n, 3)

// numtheory

size_t lmpn_gcd(limb_t *g, limb_t *a, size_t n, limb_t *b, size_t m);
limb_t lmpn_gcd_1(const limb_t *a, size_t n, limb_t b);

// str

size_t lmpn_sizeinbase(const limb_t *a, size_t n, int base);
size_t lmpn_limbs_for_digits(size_t len, int base);
size_t lmpn_get_str(unsigned char *str, const limb_t *a, size_t n, int base);
size_t lmpn_set_str(limb_t *a, const unsigned char *str, size_t str_len, int base);

/* ------ LMPZ ------ */

// constant

#define LMPZ_SIGN_BIT  (~(SIZE_MAX >> 1))

// helper

size_t lmpz_neg_size(size_t size);
size_t lmpz_gen_size(size_t size, size_t sign_bit);

// type

typedef struct {
    size_t size;
    size_t alloc;
    uintptr_t limb;
} lmpz;

typedef lmpz lmpz_t[1];

#define lmpz_cap(z) ((z)->alloc)
#define lmpz_limb(z)  ((z)->limb)
#define lmpz_ptr(z)   ((void*)((z)->limb))
#define lmpz_limbs(z) ((limb_t*)(void*)((z)->limb))
#define lmpz_at(z, i) (lmpz_limbs(z)[i])

#define lmpz_size(z)    ((z)->size & (SIZE_MAX >> 1))
#define lmpz_sgn_bit(z) ((z)->size & LMPZ_SIGN_BIT)

// memory

void lmpz_init(lmpz_t z);
void lmpz_clear(lmpz_t z);

int lmpz_reserve(lmpz_t z, size_t n);
int lmpz_init_reserve(lmpz_t z, size_t n);

void lmpz_shrink(lmpz_t z);

// query

int lmpz_sign(lmpz_t z);

int lmpz_sizeinbase(const lmpz_t z, int base);

// assignment

void _lmpz_init_set(lmpz_t z, size_t sz, size_t alloc, uintptr_t limb);

void lmpz_swap(lmpz_t a, lmpz_t b);

int lmpz_set(lmpz_t z, const lmpz_t a);
int lmpz_init_set(lmpz_t z, const lmpz_t a);

int lmpz_neg(lmpz_t z, const lmpz_t a);
int lmpz_init_neg(lmpz_t z, const lmpz_t a);

int lmpz_set_mpn(lmpz_t z, const limb_t *ap, size_t an);
int lmpz_init_set_mpn(lmpz_t z, const limb_t *ap, size_t an);

int lmpz_neg_mpn(lmpz_t z, const limb_t *ap, size_t an);
int lmpz_init_neg_mpn(lmpz_t z, const limb_t *ap, size_t an);

int lmpz_signed_mpn(lmpz_t z, const limb_t *ap, size_t an, size_t sign_bit);
int lmpz_init_signed_mpn(lmpz_t z, const limb_t *ap, size_t an, size_t sign_bit);

void lmpz_set_ui(lmpz_t z, limb_t a);
void lmpz_init_set_ui(lmpz_t z, limb_t a);

void lmpz_neg_ui(lmpz_t z, limb_t a);
void lmpz_init_neg_ui(lmpz_t z, limb_t a);

void lmpz_set_signed_ui(lmpz_t z, limb_t a, size_t sign_bit);
void lmpz_init_set_signed_ui(lmpz_t z, limb_t a, size_t sign_bit);

void lmpz_set_si(lmpz_t z, slimb_t a);
void lmpz_init_set_si(lmpz_t z, slimb_t a);

int lmpz_set_uiui(lmpz_t z, limb_t hi, limb_t lo);
int lmpz_init_set_uiui(lmpz_t z, limb_t hi, limb_t lo);

int lmpz_neg_uiui(lmpz_t z, limb_t hi, limb_t lo);
int lmpz_init_neg_uiui(lmpz_t z, limb_t hi, limb_t lo);

int lmpz_set_signed_uiui(lmpz_t z, limb_t hi, limb_t lo, size_t sign_bit);
int lmpz_init_set_signed_uiui(lmpz_t z, limb_t hi, limb_t lo, size_t sign_bit);

void lmpz_zero(lmpz_t z);
void lmpz_one(lmpz_t z);
void lmpz_negative_one(lmpz_t z);

int lmpz_set_str(lmpz_t z, const char *str, int base);

// get

limb_t lmpz_get_ui(const lmpz_t z);
slimb_t lmpz_get_si(const lmpz_t z);

const limb_t *lmpz_get_mpn(const lmpz_t z);

char *lmpz_get_str(char *str, const lmpz_t z, int base);

// compare

int lmpz_cmp(const lmpz_t a, const lmpz_t b);
int lmpz_cmp_ui(const lmpz_t a, limb_t b);
int lmpz_cmp_si(const lmpz_t a, slimb_t b);

int lmpz_equal(const lmpz_t a, const lmpz_t b);
int lmpz_equal_ui(const lmpz_t a, limb_t b);
int lmpz_equal_si(const lmpz_t a, slimb_t b);

#define lmpz_is_zero(z)        (!((z)->size))
#define lmpz_is_negative(z)    (((z)->size) > (SIZE_MAX >> 1))
#define lmpz_is_positive(z)    ((z)->size-1 < (SIZE_MAX >> 1))
#define lmpz_is_nonnegative(z) (((z)->size) <= (SIZE_MAX >> 1))
#define lmpz_is_nonpositive(z) ((z)->size-1 >= (SIZE_MAX >> 1))

int lmpz_is_one(const lmpz_t z);
int lmpz_is_pm1(const lmpz_t z);

// operation

int _lmpz_add_mpn_ui(lmpz_t z, size_t sign_bit, const limb_t *ap, size_t an, limb_t b);
int _lmpz_add_mpn_mpn(lmpz_t z, size_t sign_bit, const limb_t *ap, size_t an, const limb_t *bp, size_t bn);

int _lmpz_sub_mpn_ui(lmpz_t z, size_t sign_bit, const limb_t *ap, size_t an, limb_t b);
int _lmpz_sub_mpn_mpn(lmpz_t z, size_t sign_bit, const limb_t *ap, size_t an, const limb_t *bp, size_t bn);

int _lmpz_aors_ui(lmpz_t z, const lmpz_t a, limb_t b, size_t sign_bit);
int _lmpz_aors_mpn(lmpz_t z, const lmpz_t a, const limb_t *bp, size_t bn, size_t sign_bit);
int _lmpz_aors(lmpz_t z, const lmpz_t a, const lmpz_t b, size_t sign_bit);

#define lmpz_add(z, a, b)            _lmpz_aors(z, a, b, 0)
#define lmpz_add_ui(z, a, b)         _lmpz_aors_ui(z, a, b, 0)
#define lmpz_add_mpn(z, a, bp, bn)   _lmpz_aors_mpn(z, a, bp, bn, 0)

#define lmpz_sub(z, a, b)            _lmpz_aors(z, a, b, LMPZ_SIGN_BIT)
#define lmpz_sub_ui(z, a, b)         _lmpz_aors_ui(z, a, b, LMPZ_SIGN_BIT)
#define lmpz_sub_mpn(z, a, bp, bn)   _lmpz_aors_mpn(z, a, bp, bn, LMPZ_SIGN_BIT)

LMP_INLINE int lmpz_add_si(lmpz_t z, const lmpz_t a, slimb_t b) {return b >= 0 ? lmpz_add_ui(z, a, b) : lmpz_sub_ui(z, a, -(limb_t)b);}
LMP_INLINE int lmpz_sub_si(lmpz_t z, const lmpz_t a, slimb_t b) {return b >= 0 ? lmpz_sub_ui(z, a, b) : lmpz_add_ui(z, a, -(limb_t)b);}

int lmpz_ui_sub(lmpz_t z, limb_t a, const lmpz_t b);
int lmpz_mpn_sub(lmpz_t z, const limb_t *ap, size_t an, const lmpz_t b);

int _lmpz_mul_mpn_ui(lmpz_t z, size_t sign_bit, limb_t *ap, size_t an, limb_t b);
int _lmpz_mul_mpn_mpn(lmpz_t z, size_t sign_bit, limb_t *ap, size_t an, limb_t *bp, size_t bn);

int _lmpz_mul_ui(lmpz_t z, const lmpz_t a, limb_t b, size_t sign_bit);
int _lmpz_mul_mpn(lmpz_t z, const lmpz_t a, limb_t *bp, size_t bn, size_t sign_bit);

#define lmpz_mul_ui(z, a, b)        _lmpz_mul_ui(z, a, b, 0)
#define lmpz_mul_mpn(z, a, bp, bn)  _lmpz_mul_mpn(z, a, bp, bn, 0)

int lmpz_mul(lmpz_t z, const lmpz_t a, const lmpz_t b);
int lmpz_mul_si(lmpz_t z, const lmpz_t a, slimb_t b);

limb_t lmpz_tdiv_r_ui(const lmpz_t a, limb_t b);
limb_t lmpz_fdiv_r_ui(const lmpz_t a, limb_t b);
limb_t lmpz_cdiv_r_ui(const lmpz_t a, limb_t b);

limb_t lmpz_tdiv_qr_ui(lmpz_t q, const lmpz_t a, limb_t b);
limb_t lmpz_fdiv_qr_ui(lmpz_t q, const lmpz_t a, limb_t b);
limb_t lmpz_cdiv_qr_ui(lmpz_t q, const lmpz_t a, limb_t b);

int lmpz_tdiv_q_ui(lmpz_t q, const lmpz_t a, limb_t b);
int lmpz_fdiv_q_ui(lmpz_t q, const lmpz_t a, limb_t b);
int lmpz_cdiv_q_ui(lmpz_t q, const lmpz_t a, limb_t b);

int lmpz_tdiv_qr(lmpz_t q, lmpz_t r, const lmpz_t a, const lmpz_t b);
int lmpz_fdiv_qr(lmpz_t q, lmpz_t r, const lmpz_t a, const lmpz_t b);
int lmpz_cdiv_qr(lmpz_t q, lmpz_t r, const lmpz_t a, const lmpz_t b);

int lmpz_tdiv_q(lmpz_t q, const lmpz_t a, const lmpz_t b);
int lmpz_fdiv_q(lmpz_t q, const lmpz_t a, const lmpz_t b);
int lmpz_cdiv_q(lmpz_t q, const lmpz_t a, const lmpz_t b);

int lmpz_tdiv_r(lmpz_t r, const lmpz_t a, const lmpz_t b);
int lmpz_fdiv_r(lmpz_t r, const lmpz_t a, const lmpz_t b);
int lmpz_cdiv_r(lmpz_t r, const lmpz_t a, const lmpz_t b);

int lmpz_divexact(lmpz_t q, const lmpz_t a, const lmpz_t b);
int lmpz_divexact_ui(lmpz_t q, const lmpz_t a, limb_t b);
int lmpz_divexact_by3(lmpz_t q, const lmpz_t a);

#ifdef __cplusplus
}
#endif

#endif