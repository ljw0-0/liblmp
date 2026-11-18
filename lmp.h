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

/* ------ MPN ------ */

#define LMPN_NTT_START 10

typedef struct {limb_t p, *wk, *wki;} lmpn_ntt_prime_t;

extern const lmpn_ntt_prime_t lmpn_ntt_primes[][3];
extern const size_t lmpn_ntt_prime_count;

// memory

#define LMPN_MAX_LEN (SIZE_MAX / sizeof(limb_t))

static inline limb_t *lmpn_alloc(size_t sz) {
    return malloc(sz*sizeof(limb_t));
}

static inline limb_t *lmpn_realloc(limb_t *p, size_t sz) {
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

static inline int lmpn_add(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    return (n ^ m) ? lmpn_add_1(c+m, a+m, n-m, lmpn_add_n(c, a, b, m)) : lmpn_add_n(c, a, b, m);
}

int lmpn_sub_1(limb_t *c, const limb_t *a, size_t n, limb_t b);
int lmpn_sub_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);

static inline int lmpn_sub(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    return (n ^ m) ? lmpn_sub_1(c+m, a+m, n-m, lmpn_sub_n(c, a, b, m)) : lmpn_sub_n(c, a, b, m);
}

limb_t lmpn_addlsh1_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);
limb_t lmpn_addlsh2_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);
limb_t lmpn_addlsh_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n, int cnt);

static inline limb_t lmpn_addlsh1(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    return (n ^ m) ? (limb_t)lmpn_add_1(c+m, a+m, n-m, lmpn_addlsh1_n(c, a, b, m)) : lmpn_addlsh1_n(c, a, b, m);
}

static inline limb_t lmpn_addlsh2(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    return (n ^ m) ? (limb_t)lmpn_add_1(c+m, a+m, n-m, lmpn_addlsh2_n(c, a, b, m)) : lmpn_addlsh2_n(c, a, b, m);
}

static inline limb_t lmpn_addlsh(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m, int cnt) {
    assert(n >= m);
    return (n ^ m) ? (limb_t)lmpn_add_1(c+m, a+m, n-m, lmpn_addlsh_n(c, a, b, m, cnt)) : lmpn_addlsh_n(c, a, b, m, cnt);
}

limb_t lmpn_sublsh1_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);
limb_t lmpn_sublsh2_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n);
limb_t lmpn_sublsh_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n, int cnt);

static inline limb_t lmpn_sublsh1(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    return (n ^ m) ? (limb_t)lmpn_sub_1(c+m, a+m, n-m, lmpn_sublsh1_n(c, a, b, m)) : lmpn_sublsh1_n(c, a, b, m);
}

static inline limb_t lmpn_sublsh2(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    return (n ^ m) ? (limb_t)lmpn_sub_1(c+m, a+m, n-m, lmpn_sublsh2_n(c, a, b, m)) : lmpn_sublsh2_n(c, a, b, m);
}

static inline limb_t lmpn_sublsh(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m, int cnt) {
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

#ifdef __cplusplus
}
#endif

#endif