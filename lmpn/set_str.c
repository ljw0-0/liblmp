#include "lmp.h"
#include "impl.h"
#include <string.h>

#define SET_STR_THRESHOLD 128
#if SET_STR_THRESHOLD < 2*LIMB_WIDTH
# error "SET_STR_THRESHOLD < 2*LIMB_WIDTH"
#endif

static size_t lmpn_set_str_basic(limb_t *a, const unsigned char *str, size_t str_len, int base) {
    size_t n = 1;
    *a = *str;
    while (--str_len) {
        limb_t u = lmpn_mul_1(a, a, n, base);
        if (u) {a[n] = u; ++n;}
        if (lmpn_add_1(a, a, n, *++str)) {a[n] = 1; ++n;}
    }
    return n;
}

typedef struct {
    uintptr_t a;
    size_t n;
} dc_t;

static size_t _lmpn_set_str_dc(limb_t *a, const unsigned char *str, size_t len, int base, const dc_t *pow) {
    if (len < SET_STR_THRESHOLD) return lmpn_set_str_basic(a, str, len, base);
    size_t l = len>>1;
    limb_t *tmp = malloc((pow->n+1)*sizeof(limb_t));
    unlikely(!tmp) return 0;
    size_t sz = _lmpn_set_str_dc(tmp, str, l, base, pow+1);
    assert(sz <= pow->n);
    unlikely(!sz || lmpn_mul(a, (const limb_t*)(const void*)pow->a, pow->n, tmp, sz)) {tfree(tmp); return 0;}
    size_t msz = pow->n + sz;
    sz = _lmpn_set_str_dc(tmp, str+l, l, base, pow+1);
    unlikely(!sz) goto cltag(tmp);
    lmpn_add(a, a, msz, tmp, sz);
    free(tmp);
    while(msz && !a[--msz]); ++msz;
    if (len&1) {
        limb_t ad = lmpn_mul_1(a, a, msz, base);
        ad += lmpn_add_1(a, a, msz, str[len-1]);
        if (ad) {a[msz] = ad; ++msz;}
    }
    return msz;
}

static size_t lmpn_set_str_dc(limb_t *a, const unsigned char *str, size_t str_len, int base) {
    dc_t pow[SIZE_WIDTH];
    int k = 0;
    for (size_t i=str_len>>1;i;i>>=1) ++k;
    pow[--k] = (dc_t){base, 1};
    limb_t b = base;
    while (b<=LIMB_MAX/b) {
        if ((str_len >> k) & 1) {
            if (b*b > LIMB_MAX/base) break;
            b *= b * base;
        } else b *= b;
        pow[--k] = (dc_t){b, 1};
    }
    limb_t len3[3];
    limb_mul(len3+1, len3, b, b);
    if ((str_len >> k) & 1) {
        limb_t tmp;
        limb_mul(&tmp, len3, *len3, base);
        limb_mul(len3+2, len3+1, len3[1], base);
        len3[2] += ((len3[1] += tmp) < tmp);
        pow[--k] = (dc_t){(uintptr_t)(const void*)len3, 3-!(len3[2])};
    } else pow[--k] = (dc_t){(uintptr_t)(const void*)len3, 2};
    while(k) {
        size_t m = pow[k].n << 1 | 1;
        limb_t *t = malloc(m*sizeof(limb_t));
        unlikely(!t || lmpn_sqr(t, (const limb_t*)(const void*)pow[k].a, pow[k].n)) {
            while (pow[k].n > 2) {free((void*)pow[k].a); ++k;}
            return 0;
        }

        if ((str_len >> k) & 1) t[pow[k].n<<1] = lmpn_mul_1(t, t, pow[k].n << 1, base);
        else --m;
        while (m && !t[--m]);
        pow[--k] = (dc_t){(uintptr_t)(const void*)t, m+1};
        assert(m>1);
    }
    size_t ret = _lmpn_set_str_dc(a, str, str_len, base, pow);
    for (size_t i=0;(void*)pow[i].a != (void*)len3;++i) free((void*)pow[i].a);
    return ret;
}

size_t lmpn_set_str(limb_t *a, const unsigned char *str, size_t str_len, int base) {
    while (str_len && !*str) {
        --str_len;
        ++str;
    }
    if (str_len == 0) {
        a[0] = 0;
        return 1;
    }
    if (!(base & (base - 1))) {
        int k = log2n(base);

        size_t total_bits = str_len * (size_t)k;
        size_t n = (total_bits - 1) / LIMB_WIDTH + 1;
        memset(a, 0, n * sizeof(*a));

        size_t bit_pos = 0;
        for (size_t i = str_len; i-- > 0; ) {
            size_t limb_index = bit_pos / LIMB_WIDTH;
            unsigned int bit_offset = (unsigned int)(bit_pos % LIMB_WIDTH);
            limb_t digit = str[i];

            a[limb_index] |= digit << bit_offset;
            if (bit_offset + (unsigned int)k > LIMB_WIDTH) a[limb_index + 1] |= digit >> (LIMB_WIDTH - bit_offset);
            bit_pos += (size_t)k;
        }

        return lmpn_norm(a, n);
    }
    return str_len < SET_STR_THRESHOLD ? lmpn_set_str_basic(a, str, str_len, base) : lmpn_set_str_dc(a, str, str_len, base);
}
