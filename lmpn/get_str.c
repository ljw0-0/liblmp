#include "lmp.h"
#include "impl.h"
#include <assert.h>
#include <string.h>
#include <limits.h>

#define GET_STR_THRESHOLD 100
#if GET_STR_THRESHOLD < LIMB_WIDTH
# error "GET_STR_THRESHOLD < LIMB_WIDTH"
#endif

static size_t lmpn_get_str_basic(unsigned char *str, const limb_t *a, size_t n, int base) {
    limb_t *tmp = lmpn_alloc(n);
    size_t len = 0;
    if (!tmp) return 0;
    memcpy(tmp, a, n * sizeof(limb_t));
    while (n) {
        str[len++] = lmpn_div_qr_1(tmp, tmp, n, base);
        if (tmp[n-1] == 0) --n;
    }
    free(tmp);
    for (unsigned char *r = str + len - 1; str < r; --r) {
        unsigned char tmpc = *r;
        *r = *str;
        *str = tmpc;
        ++str;
    }
    return len;
}

typedef struct {
    uintptr_t a;
    size_t n;
} dc_t;

static size_t _lmpn_get_str_dc(unsigned char *str, const limb_t *a, size_t n,
                              int base, const dc_t *pow, size_t k, int pad) {
    if (n == 0) {
        if (pad) { memset(str, 0, k); return k; }
        return 0;
    }

    if (n <= GET_STR_THRESHOLD || k <= 1) {
        size_t len = lmpn_get_str_basic(str, a, n, base);
        if (len == 0) {
            if (pad) { memset(str, 0, k); return k; }
            return 0;
        }
        if (pad && len < k) {
            memmove(str + (k - len), str, len);
            memset(str, 0, k - len);
            return k;
        }
        return len;
    }

    size_t k_half = k >> 1;

    int cmp = (n < pow->n) ? -1 : (n > pow->n) ? 1 : lmpn_cmp(a, (limb_t *)pow->a, n);
    if (cmp < 0) {
        size_t len = _lmpn_get_str_dc(str, a, n, base, pow + 1, k_half, 0);
        if (!len) return 0;
        if (pad && len < k) {
            memmove(str + (k - len), str, len);
            memset(str, 0, k - len);
            return k;
        }
        return len;
    }

    limb_t *q = lmpn_alloc(n - pow->n + 1);
    if (LMP_UNLIKELY(!q)) return 0;
    limb_t *r = lmpn_alloc(pow->n);
    if (LMP_UNLIKELY(!r)) { free(q); return 0; }

    lmpn_div_qr(q, r, a, n, (limb_t *)pow->a, pow->n);

    size_t qn = n - pow->n + 1;
    while (qn > 0 && q[qn-1] == 0) --qn;
    size_t rn = pow->n;
    while (rn > 0 && r[rn-1] == 0) --rn;

    size_t qlen = _lmpn_get_str_dc(str, q, qn, base, pow + 1, k_half, pad);
    if (LMP_UNLIKELY(!qlen)) { free(q); free(r); return 0; }

    size_t rlen = _lmpn_get_str_dc(str + qlen, r, rn, base, pow + 1, k_half, 1);
    free(q);
    free(r);
    if (LMP_UNLIKELY(!rlen)) return 0;

    return qlen + rlen;
}

static size_t lmpn_get_str_dc(unsigned char *str, const limb_t *a, size_t n, int base) {
    int bits_per_digit = 0;
    for (size_t t = base; t > 1; t >>= 1) ++bits_per_digit;
    size_t L_est = ((size_t)n * LIMB_WIDTH + bits_per_digit - 1) / bits_per_digit + 1;

    // 向上取整到 2 的幂
    size_t L = 1;
    while (L < L_est) L <<= 1;
    if (L < 2) L = 2;

    dc_t pow[SIZE_WIDTH];
    int k = 0;
    for (size_t t = L >> 1; t; t >>= 1) ++k;

    pow[--k] = (dc_t){base, 1};
    limb_t b = base;
    while (b <= LIMB_MAX / b) {
        if ((L >> k) & 1) {
            if (b * b > LIMB_MAX / base) break;
            b *= b * base;
        } else b *= b;
        pow[--k] = (dc_t){b, 1};
    }

    limb_t len3[3];
    limb_mul(len3 + 1, len3, b, b);
    if ((L >> k) & 1) {
        limb_t tmp;
        limb_mul(&tmp, len3, *len3, base);
        limb_mul(len3 + 2, len3 + 1, len3[1], base);
        len3[2] += ((len3[1] += tmp) < tmp);
        pow[--k] = (dc_t){(uintptr_t)len3, 3 - !(len3[2])};
    } else pow[--k] = (dc_t){(uintptr_t)len3, 2};

    while (k) {
        size_t m = pow[k].n << 1 | 1;
        limb_t *t = malloc(m * sizeof(limb_t));
        if (LMP_UNLIKELY(!t || lmpn_sqr(t, (limb_t *)pow[k].a, pow[k].n))) {
            while (pow[k].n > 2) { free((void *)pow[k].a); ++k; }
            return 0;
        }
        if ((L >> k) & 1) t[pow[k].n << 1] = lmpn_mul_1(t, t, pow[k].n << 1, base);
        else --m;
        while (m && !t[--m]);
        pow[--k] = (dc_t){(uintptr_t)t, m + 1};
        assert(m > 1);
    }

    size_t ret = _lmpn_get_str_dc(str, a, n, base, pow, L, 0);
    for (size_t i = 0; pow[i].a != (uintptr_t)len3; ++i) free((void *)pow[i].a);
    return ret;
}

size_t lmpn_get_str(unsigned char *str, const limb_t *a, size_t n, int base) {
    size_t len = 0;
    if (base - 1 == UCHAR_MAX) {
        const limb_t *p = a + n - 1;
        limb_t b = *p;
        int i = sizeof(limb_t) - 1;
        while (!(b >> (i * CHAR_BIT)) && i) --i;
        while (i >= 0) {
            str[len++] = (b >> (i * CHAR_BIT)) & UCHAR_MAX;
            --i;
        }
        for (--p; p >= a; --p) {
            b = *p;
            for (i = sizeof(limb_t) - 1; i >= 0; --i) str[len++] = (b >> (i * CHAR_BIT)) & UCHAR_MAX;
        }
    } else if (!(base & (base - 1))) {
        size_t bits = n * LIMB_WIDTH - limb_clz(a[n-1]);
        int k = log2n(base);
        const limb_t *p = a + n - 1;
        limb_t b = *p;
        if (LIMB_WIDTH % k == 0) {
            const int div = LIMB_WIDTH / k, lmm = (bits - (n-1) * LIMB_WIDTH - 1) / k + 1;
            len = lmm;
            for (int _c = 1; _c <= lmm; ++_c) {
                str[len - _c] = b & (base - 1);
                b >>= k;
            }
            for (--p; p >= a; --p) {
                b = *p;
                len += div;
                for (int _c = 1; _c <= div; ++_c) {
                    str[len - _c] = b & (base - 1);
                    b >>= k;
                }
            }
        } else {
            unsigned char lst;
            int diff;
            int i;
            const int b_high = bits % k, lmm = bits - (n-1) * LIMB_WIDTH - b_high;
            if (b_high) { str[0] = b >> lmm; len = 1; } else len = 0;
            i = lmm - k;
            for (; i >= k; i -= k) str[len++] = (b >> i) & (base - 1);
            if (i >= 0) lst = b << (diff = k - i);
            else lst = 0, diff = k;
            for (--p; p >= a; --p) {
                b = *p;
                str[len++] = lst | (b >> (LIMB_WIDTH - diff));
                i = LIMB_WIDTH - diff - k;
                for (; i >= k; i -= k) str[len++] = (b >> i) & (base - 1);
                if (i >= 0) lst = b << (diff = k - i);
                else lst = 0, diff = k;
            }
            assert(lst == 0);
        }
    } else if (n <= GET_STR_THRESHOLD) len = lmpn_get_str_basic(str, a, n, base);
    else len = lmpn_get_str_dc(str, a, n, base);
    if (len == 0) return 0;
    str[len] = '\0';
    return len;
}